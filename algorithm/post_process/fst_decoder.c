#include <stdio.h>
#include <float.h>
#include <string.h>
#include "fst_nth.h"
#include "fst_decoder.h"

#define INT_MAX (0x3FFFFFFF)

//static const int beam = 20;
//static const float beam_delta = 0.5;
static const int beam = 20*32768;    // 20@Q15
static const int beam_delta = 16384; // 0.5@Q15
static const int min_active = 20;    // 最小活跃数
static const int max_active = 1000;  // 最大活跃数
static const float acoustic_scale = 1; //声学代价权重

#if 0
inline 
int partition(int32_t* a, int low, int high) {
  int i = low;
  int j = high;
  int32_t key = a[low];
  int32_t temp;
  while (i < j) {
    while(i < j && a[j] >= key) j--;
    while(i < j && a[i] <= key) i++;
    if (i < j) {
      temp = a[i];
      a[i] = a[j];
      a[j] = temp;
    }
  }
  a[low] = a[i];
  a[i] = key;
  return i;
}

inline
int32_t nth(int32_t *a, int low, int high, int n) {
  int mid = partition(a, low, high);
  if (mid == n) return a[mid];
  return
    (mid < n) ?
    nth(a, mid+1, high, n) :
    nth(a, low, mid-1, n);
}

#endif

void decoder_copy_toks(Decoder *dest, Decoder *src, Fsts* fsts) {
  int i;
  for (i = 0; i < fsts->num_states; i++) {
    token_copy(&dest->cur_toks[i], &src->cur_toks[i]);
  }
  dest->decoded_frames = src->decoded_frames;
}

void decoder_reset(Decoder *decoder, Fsts* fsts) {
  int i;
#if 1
  for (i = 0; i < fsts->num_states; i++) {
    token_reset(&decoder->pre_toks[i]);
    token_reset(&decoder->cur_toks[i]);
  }    
  decoder->cur_toks[0].active = 1;
  decoder->state_ids_len  = 0;
  decoder->decoded_frames = 0;
#endif
}

int get_cutoff(Decoder *decoder, int32_t *adaptive_beam, int32_t *best_state, Fsts* fsts) {
  int i;
  int32_t best_cost = INT_MAX;
  int cutoff_len = 0;
  //找出当前帧的最优token,即最小代价的token
  for (i = 0; i < fsts->num_states; i++) {
    if (decoder->pre_toks[i].active) {
      int w = decoder->pre_toks[i].cost;
      decoder->cutoff[cutoff_len++] = w;
      if (w < best_cost) {
        best_cost = w;
        *best_state = i;
      }
    }
  }

  //根据最优token设定剪枝上限，即beam_cutoff = best_cost + beam
  //设定最大活跃节点数，即max_active_cutoff
  //设定最小活跃节点数，即min_active_cutoff

  int32_t beam_cutoff = best_cost + beam,
        min_active_cutoff = INT_MAX,
        max_active_cutoff = INT_MAX;
  if (cutoff_len > max_active) {
    max_active_cutoff = nth(decoder->cutoff, 0, cutoff_len-1, max_active);
  }
  if (max_active_cutoff < beam_cutoff) {
      if (adaptive_beam)
        *adaptive_beam = max_active_cutoff - best_cost + beam_delta;
      return max_active_cutoff;
  }
  if (cutoff_len > min_active) {
    if (min_active == 0) min_active_cutoff = best_cost;
    else {
      min_active_cutoff = nth(decoder->cutoff, 0,
                              cutoff_len > max_active ?
                              max_active-1 : cutoff_len-1,
                              min_active);
    }
  }
  if (min_active_cutoff > beam_cutoff) {
    if (adaptive_beam) {
      *adaptive_beam = min_active_cutoff - best_cost + beam_delta;
    }
    return min_active_cutoff;
  } else {
    *adaptive_beam = beam;
    return beam_cutoff;
  } 
}

int process_emitting(Decoder *decoder, int16_t *likes, Fsts* fsts) {
  int i, j;
  int32_t adaptive_beam;
  int best_state = -1;
  //计算剪枝上限/阈值：weight_cutoff
  int32_t weight_cutoff = get_cutoff(decoder, &adaptive_beam, &best_state, fsts);
  //计算扩张剪枝上限/阈值：对最优token进行后续一帧的扩张，计算每个转移弧新的cost,结合beam阈值得到后续扩张的剪枝上限next_weight_cutoff
  int32_t next_weight_cutoff = INT_MAX;
  if (best_state >= 0) {
    int sid = best_state;
    const Arc* arcs = fsts->Arcs[sid];
    for (i = 0; i < fsts->num_arcs[sid]; i++) {
      Arc arc = arcs[i];
      if (arc.ilabel != 0) {
        int32_t ac_cost = -(likes[arc.ilabel-1]<<(fsts->weight_am_shift));
        int32_t new_weight = arc.weight + decoder->pre_toks[sid].cost + ac_cost;
        if (new_weight + adaptive_beam < next_weight_cutoff) { 
          next_weight_cutoff = new_weight + adaptive_beam;
        }
      }
    }
  }

  //遍历s_id，即state_id
  for (i = 0; i < fsts->num_states; i++) {
    Token *tok = &decoder->pre_toks[i];
    //第一轮剪枝：对当前帧的所有token做剪枝，抑制代价超过weight_cutoff的token
    if (!tok->active || tok->cost >= weight_cutoff) continue;
    //遍历状态的连接弧
    const Arc *arcs = fsts->Arcs[i];
    for (j = 0; j < fsts->num_arcs[i]; j++) {
      Arc arc = arcs[j];
      if (arc.ilabel != 0) {
        int32_t ac_cost = -(likes[arc.ilabel-1]<<(fsts->weight_am_shift));
        int32_t new_weight = arc.weight + tok->cost + ac_cost;
        //第二轮剪枝：对于当前帧除最优token之外的其他token,根据后续的转移弧计算更新的cost,如果超过扩张剪枝上限，则该转移弧不再扩张
        if (new_weight < next_weight_cutoff) {
          //继续更新扩张剪枝上限/阈值，这是一个不断估计的过程
          if (new_weight + adaptive_beam < next_weight_cutoff) {
            next_weight_cutoff = new_weight + adaptive_beam;
          }
          //产生新的token
          Token *new_tok = &decoder->cur_toks[arc.nextstate];
          if (!new_tok->active || new_tok->cost > new_weight) {
            token_copy(new_tok, tok);
            new_tok->cost = new_weight;
            // skip <eps> and <sil>
            if (arc.olabel > 0 && arc.olabel != fsts->sil_index) {
              new_tok->olabel = arc.olabel;
            }
            // 当前转移弧对应的音素非<eps>
            if (arc.phone_id > 0) {
              if (arc.phone_id == new_tok->phone_id) {
                //token的音素与arc音素相同，则计数+1
                new_tok->phone_count++;
              } else {
                if (new_tok->phone_id != 0) {
                  new_tok->phone_length++;
                  if (new_tok->phone_length < MAX_PHONE_FRAMES_LEN) {
                    new_tok->phone_frames[new_tok->phone_length] = new_tok->phone_count;
                  }
                  else {
                    for (int k = 1; k < MAX_PHONE_FRAMES_LEN; k++) {
                      new_tok->phone_frames[k-1] = new_tok->phone_frames[k];
                    }
                    new_tok->phone_frames[MAX_PHONE_FRAMES_LEN-1] = new_tok->phone_count;
                  } 
                }
                new_tok->phone_id = arc.phone_id;
                new_tok->phone_count = 1;
              }
            }
          } 
        } 
      }
    }
  }
  return next_weight_cutoff;
}

void process_nonemitting(Decoder *decoder, int32_t cutoff, Fsts* fsts) {
  int i, j;
  decoder->state_ids_len = 0;
  for (i = 0; i < fsts->num_states; i++) {
    if (decoder->cur_toks[i].active) {
      decoder->state_ids[decoder->state_ids_len++] = i;
    }
  }
  while (decoder->state_ids_len > 0) {
    int sid = decoder->state_ids[--decoder->state_ids_len];
    Token *tok = &decoder->cur_toks[sid];
    if (!tok->active || tok->cost >= cutoff) continue; 
    const Arc *arcs = fsts->Arcs[sid];
    for (i = 0; i < fsts->num_arcs[sid]; i++) {
      Arc arc = arcs[i];
      //处理非发射转移弧
      if (arc.ilabel == 0) {
        //更新累计代价
        int32_t new_cost = tok->cost + arc.weight;
        if (new_cost < cutoff) {
          Token *new_tok = &decoder->cur_toks[arc.nextstate];
          if (!new_tok->active || new_tok->cost > new_cost) {
            token_copy(new_tok, tok);
            new_tok->cost = new_cost;
            // skip <eps> and <sil>
            if (arc.olabel > 0 && arc.olabel != fsts->sil_index) {
              new_tok->olabel = arc.olabel;
            }
            decoder->state_ids[decoder->state_ids_len++] = arc.nextstate;
          }
        }
      }
    }
  }
}

void decoder_decode(Decoder *decoder, int16_t *likes, Fsts* fsts) {
  int i;
  for (i = 0; i < fsts->num_states; i++) {
    /*拷贝当前token至pretoken*/
   token_copy(&decoder->pre_toks[i], &decoder->cur_toks[i]);
    /*重置当前token*/
   token_reset(&decoder->cur_toks[i]);
  }
  int32_t weight_cutoff = process_emitting(decoder, likes, fsts);
#if 1
  process_nonemitting(decoder, weight_cutoff, fsts);
  decoder->decoded_frames++;
#endif
}

#if 0
int fst_is_final(int sid) {
  int i;
  for (i = 0; i < NUM_FINALS; i++) {
    if (sid == final_states[i]) return 1; 
  }
  return 0;
}
#endif
int decoder_get_result(Decoder *decoder, int32_t *olablel, int32_t *score, Fsts* fsts) {
  int i;
  int best_sid = -1;
  int32_t best_cost = INT_MAX;
  for (i = 0; i < fsts->num_finals; i++){
    int final_sid = fsts->final_states[i];
    Token *tok  = &decoder->cur_toks[final_sid];
    if(tok->active && tok->cost < best_cost){
      best_cost = tok->cost;
      best_sid = final_sid;
    }
  }
#if 0
  for (i = 0; i < fsts->num_states; i++) {
    Token *tok = &decoder->cur_toks[i];
    if (tok->active && fst_is_final(i) && tok->cost < best_cost) {
      best_cost = tok->cost;
      best_sid = i;
    }
  }
#endif
  if (best_sid == -1) {
    return 0;
  }
  Token *best_tok = &decoder->cur_toks[best_sid];
  if (best_tok->olabel == 0 || best_tok->olabel == fsts->sil_index ) {
    return 0;
  }

#if 0
  int word_frames = 0;
  for (i = 1; i < best_tok->phone_length; i += 2) {
    word_frames = best_tok->phone_frames[i] + best_tok->phone_frames[i+1];
    if (word_frames <= 2) break;
  } 
  if (word_frames <= 2) {
    return 0;
  }
#else
  int w1 = 0, w2 = 0;
  int k = min(MAX_PHONE_FRAMES_LEN, best_tok->phone_length);
  w1 = best_tok->phone_frames[k-1] + best_tok->phone_frames[k-2];
  w2 = best_tok->phone_frames[k-3] + best_tok->phone_frames[k-4];
  if (w1 <= 2 || w2 <= 2) {
    return 0;
  }
#endif

  *olablel = best_tok->olabel - 1;
  //*text = fsts->words[best_tok->olabel];

  //if (strcmp(*text, "preWord") == 0) return 0;

  int32_t total_cost = (-best_tok->cost);
  if (decoder->decoded_frames > 0) {
    *score = total_cost / decoder->decoded_frames;
  } else {
    *score = 0;
  }

  return 1;
}
