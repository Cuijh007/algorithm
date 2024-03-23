#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __IET_MFCCDATA_H__
#define __IET_MFCCDATA_H__
#define NUM_FBANK 64

typedef struct __attribute__((packed))
{
    short frameSize;       /* speech frameSize */
    short numChans;        /* number of channels */
    short sampPeriod;     /* sample period */
    short fftN;
    short klo;
    short khi;       /* lopass to hipass cut-off fft indices */
    short loChan[257];/* array[1..fftN/2] of loChan index */
    short loWt[257];  /* array[1..fftN/2] of loChan weighting */
}
FBankInfo_FIXED;
struct __attribute__((packed)) post_proc_buf_t
{
    int mfcc_out[NUM_FBANK*2];
    short fbank_k[NUM_FBANK];
    int fbank_b[NUM_FBANK];
};

struct post_proc_buf_t __attribute__((aligned(16))) post_proc_buf;
const struct post_proc_buf_t post_proc_buf_rodata =
{
    .fbank_k = {  //Q10
        382, 420, 392, 358, 343, 332, 316, 301,
        290, 280, 268, 256, 242, 228, 219, 214,
        211, 208, 201, 200, 195, 195, 186, 186,
        183, 188, 187, 188, 184, 180, 178, 184,
        185, 186, 186, 190, 191, 189, 191, 198,
        199, 199, 205, 207, 216, 228, 234, 238,
        246, 254, 270, 285, 289, 302, 320, 319,
        361, 348, 404, 462, 383, 310, 294, 333
    },
    .fbank_b = { //Q20
        -3482651, -2446237, -2158565, -1819962, -1748405, -1774066, -1762391, -1787270,
            -1730079, -1695913, -1688087, -1571791, -1498605, -1387426, -1382097, -1357138,
            -1378654, -1404371, -1404713, -1447514, -1428074, -1475186, -1407996, -1433400,
            -1415372, -1495340, -1467324, -1507620, -1455088, -1414370, -1389514, -1469524,
            -1502802, -1541670, -1553878, -1598570, -1605144, -1583515, -1590051, -1649478,
            -1638592, -1634119, -1676895, -1710243, -1793060, -1925100, -1976685, -2007529,
            -2042951, -2107211, -2248093, -2400822, -2462619, -2566242, -2898958, -2517175,
            -3380953, -2503728, -3675888, -3616295, -3094493, -2740933, -2498989, -2741460
        },
};

const FBankInfo_FIXED __attribute__((aligned(16))) fb;
const FBankInfo_FIXED __attribute__((aligned(16))) fb_rodata=
{
    .frameSize = 1024,
    .numChans = NUM_FBANK,
    .sampPeriod = 625,
    .fftN = 512,
    .klo = 5,
    .khi = 256,
    .loChan = {//0},
        -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,
            6,  7,  8,  8,  9, 10, 11, 11, 12, 13,
            13, 14, 14, 15, 16, 16, 17, 17, 18, 18,
            19, 20, 20, 21, 21, 21, 22, 22, 23, 23,
            24, 24, 25, 25, 25, 26, 26, 27, 27, 27,
            28, 28, 29, 29, 29, 30, 30, 30, 31, 31,
            31, 32, 32, 32, 33, 33, 33, 34, 34, 34,
            34, 35, 35, 35, 36, 36, 36, 36, 37, 37,
            37, 38, 38, 38, 38, 39, 39, 39, 39, 40,
            40, 40, 40, 41, 41, 41, 41, 41, 42, 42,
            42, 42, 43, 43, 43, 43, 43, 44, 44, 44,
            44, 44, 45, 45, 45, 45, 45, 46, 46, 46,
            46, 46, 47, 47, 47, 47, 47, 48, 48, 48,
            48, 48, 48, 49, 49, 49, 49, 49, 49, 50,
            50, 50, 50, 50, 50, 51, 51, 51, 51, 51,
            51, 52, 52, 52, 52, 52, 52, 53, 53, 53,
            53, 53, 53, 53, 54, 54, 54, 54, 54, 54,
            54, 55, 55, 55, 55, 55, 55, 55, 56, 56,
            56, 56, 56, 56, 56, 56, 57, 57, 57, 57,
            57, 57, 57, 58, 58, 58, 58, 58, 58, 58,
            58, 59, 59, 59, 59, 59, 59, 59, 59, 59,
            60, 60, 60, 60, 60, 60, 60, 60, 61, 61,
            61, 61, 61, 61, 61, 61, 61, 62, 62, 62,
            62, 62, 62, 62, 62, 62, 62, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64
        },
    .loWt = {   // *1024
        0,    0,    0,    0,  165,  152,  177,  235,  326,  448,  597,
        774,  975,  176,  423,  691,  979,  262,  587,  929,  263,  637,
        2,  404,  821,  226,  668,   97,  562,   14,  501,  998,  480,
        996,  497,    7,  549,   76,  634,  176,  750,  306,  894,  464,
        41,  648,  237,  856,  457,   64,  700,  317,  964,  592,  224,
        885,  527,  174,  849,  504,  164,  851,  519,  190,  889,  569,
        251,  962,  651,  344,   41,  765,  468,  174,  907,  619,  334,
        52,  797,  521,  247, 1000,  731,  466,  202,  965,  707,  451,
        197,  969,  720,  473,  228, 1009,  769,  530,  294,   59,  851,
        620,  391,  164,  963,  740,  518,  298,   80,  888,  673,  460,
        249,   39,  855,  648,  443,  239,   37,  860,  660,  462,  266,
        71,  901,  708,  517,  327,  139,  976,  789,  605,  421,  239,
        57,  901,  722,  545,  368,  192,   18,  869,  696,  525,  355,
        186,   18,  875,  709,  543,  379,  216,   54,  917,  756,  597,
        438,  280,  124,  992,  837,  683,  529,  377,  225,   74,  948,
        799,  651,  503,  356,  210,   65,  944,  800,  657,  515,  373,
        232,   92,  976,  838,  699,  562,  425,  289,  154,   19,  909,
        775,  642,  510,  378,  247,  117, 1011,  882,  753,  625,  498,
        371,  245,  119, 1018,  894,  770,  646,  523,  401,  279,  158,
        37,  941,  821,  702,  583,  465,  347,  230,  113, 1021,  905,
        790,  675,  560,  446,  333,  220,  108, 1019,  908,  797,  686,
        576,  466,  356,  247,  139,   31,  947,  840,  733,  626,  520,
        415,  309,  204,  100, 1020,  916,  813,  710,  607,  505,  403,
        302,  201,  100
    },
};
const int32_t __attribute__((aligned(16))) prep_out[] =
{
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    -1024, 0,
    0, 0,
    0, 0,
    -1023, 0,
    0, 0,
    0, 0,
    0, 0,
    1021, 0,
    0, 0,
    0, 0,
    0, 0,
    1018, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    -1009, 0,
    0, 0,
    -1006, 0,
    2008, 0,
    -2006, 0,
    2002, 0,
    -2997, 0,
    3988, 0,
    -2985, 0,
    4965, 0,
    -2973, 0,
    4945, 0,
    -3948, 0,
    3940, 0,
    -982, 0,
    -3920, 0,
    11724, 0,
    -34125, 0,
    92340, 0,
    -287120, 0,
    1900155, 0,
    22052464, 0,
    7413154, 0,
    701256, 0,
    -3139085, 0,
    -7580776, 0,
    -10188464, 0,
    -11639584, 0,
    -11058561, 0,
    -9026607, 0,
    -5425992, 0,
    -1198905, 0,
    3331394, 0,
    7191316, 0,
    10035048, 0,
    11203529, 0,
    10816215, 0,
    8689118, 0,
    5315927, 0,
    1112496, 0,
    -3185100, 0,
    -6993280, 0,
    -9676416, 0,
    -10852248, 0,
    -10413219, 0,
    -8403240, 0,
    -5110875, 0,
    -1087008, 0,
    3083052, 0,
    6717566, 0,
    9317022, 0,
    10417689, 0,
    10019049, 0,
    8063576, 0,
    4911480, 0,
    1038740, 0,
    -2950050, 0,
    -6438670, 0,
    -8911955, 0,
    -9966624, 0,
    -9569800, 0,
    -7710078, 0,
    -4687690, 0,
    -990671, 0,
    2813184, 0,
    6135452, 0,
    8490174, 0,
    9490278, 0,
    9110372, 0,
    7319130, 0,
    4448806, 0,
    940464, 0,
    -2666801, 0,
    -5815070, 0,
    -8033440, 0,
    -8976555, 0,
    -8614000, 0,
    -6917096, 0,
    -4202555, 0,
    -886259, 0,
    2514816, 0,
    5472792, 0,
    7567329, 0,
    8439874, 0,
    8095486, 0,
    6497400, 0,
    3946050, 0,
    832236, 0,
    -2354313, 0,
    -5129110, 0,
    -7078112, 0,
    -7889598, 0,
    -7563800, 0,
    -6066790, 0,
    -3676505, 0,
    -775632, 0,
    2195136, 0,
    4771152, 0,
    6579342, 0,
    7340614, 0,
    7021595, 0,
    5627895, 0,
    3408218, 0,
    719032, 0,
    -2031172, 0,
    -4411970, 0,
    -6079360, 0,
    -6766002, 0,
    -6478200, 0,
    -5187822, 0,
    -3138765, 0,
    -660033, 0,
    1864800, 0,
    4053920, 0,
    5580498, 0,
    6204712, 0,
    5924102, 0,
    4748835, 0,
    2870386, 0,
    603340, 0,
    -1700929, 0,
    -3694830, 0,
    -5080608, 0,
    -5642406, 0,
    -5392600, 0,
    -4308854, 0,
    -2601025, 0,
    -546920, 0,
    1541568, 0,
    3336688, 0,
    4592511, 0,
    5093238, 0,
    4862012, 0,
    3879330, 0,
    2344246, 0,
    491380, 0,
    -1384890, 0,
    -2993280, 0,
    -4114424, 0,
    -4567662, 0,
    -4342400, 0,
    -3468102, 0,
    -2092510, 0,
    -438779, 0,
    1232544, 0,
    2666232, 0,
    3658809, 0,
    4055048, 0,
    3858927, 0,
    3076710, 0,
    1853182, 0,
    388128, 0,
    -1090157, 0,
    -2354090, 0,
    -3224232, 0,
    -3566196, 0,
    -3386600, 0,
    -2694228, 0,
    -1624910, 0,
    -339339, 0,
    951936, 0,
    2058144, 0,
    2811963, 0,
    3114570, 0,
    2950250, 0,
    2350530, 0,
    1414732, 0,
    294828, 0,
    -827383, 0,
    -1785055, 0,
    -2442600, 0,
    -2686860, 0,
    -2548800, 0,
    -2025448, 0,
    -1215760, 0,
    -253572, 0,
    713952, 0,
    1535812, 0,
    2095401, 0,
    2308446, 0,
    2194986, 0,
    1739010, 0,
    1040588, 0,
    217700, 0,
    -610772, 0,
    -1309560, 0,
    -1791240, 0,
    -1978506, 0,
    -1864400, 0,
    -1480870, 0,
    -888440, 0,
    -185207, 0,
    518592, 0,
    1114828, 0,
    1530837, 0,
    1685532, 0,
    1593135, 0,
    1261260, 0,
    759980, 0,
    157988, 0,
    -443875, 0,
    -950990, 0,
    -1302720, 0,
    -1441134, 0,
    -1368800, 0,
    -1089156, 0,
    -654640, 0,
    -136730, 0,
    383616, 0,
    826376, 0,
    1129128, 0,
    1245828, 0,
    1191901, 0,
    945945, 0,
    572908, 0,
    119424, 0,
    -337345, 0,
    -724935, 0,
    -998752, 0,
    -1111383, 0,
    -1062000, 0,
    -850306, 0,
    -514360, 0,
    -108141, 0,
    305472, 0,
    662660, 0,
    922845, 0,
    1025976, 0,
    991284, 0,
    793065, 0,
    485218, 0,
    102008, 0,
    -291182, 0,
    -639190, 0,
    -890192, 0,
    -1001466, 0
};

const int32_t ref_mfcc_out[257*2] =
{
    1724107, 0,
    -2257506, 374178,
    1993672, -574405,
    -2644641, 1841768,
    1800381, -2451651,
    -1374328, 4347192,
    -902381, -4343607,
    2680386, 5355585,
    -5682129, -2923211,
    6992542, 1815632,
    -8315766, 3342434,
    6521657, -5546337,
    -4038859, 11020025,
    -1671177, -10622538,
    6746262, 12495666,
    -13267348, -6143051,
    16137250, 2686753,
    -17700706, 9102309,
    12886442, -14170013,
    -5834007, 25299445,
    -7490057, -23553758,
    19624937, 25335821,
    -33561551, -9324413,
    39133082, -1499039,
    -40003458, 30490916,
    24972696, -45296914,
    -1452221, 72835118,
    -41476090, -69693200,
    90706028, 67042765,
    -157756386, -10168035,
    244436956, -135215569,
    -947949, 501628533,
    -600333265, -165984901,
    270135848, -448421664,
    148301087, 268185033,
    -163427985, -90183679,
    141467381, -10486647,
    -96091313, 50567407,
    52128296, -88450155,
    -2825453, 78976572,
    -30557834, -73598786,
    56824728, 36564878,
    -61361875, -13547208,
    56930269, -23818391,
    -35861635, 37479513,
    13729862, -54164562,
    13456959, 44869290,
    -30893025, -37837735,
    43814397, 12063736,
    -41979976, 4119945,
    34343485, -27991057,
    -15852151, 34401908,
    -1636627, -41568980,
    21143781, 30090781,
    -31399520, -20568696,
    37053356, -1245218,
    -30676424, 13978389,
    20549071, -30398451,
    -2904640, 31770678,
    -11608308, -32566122,
    25853979, 19042640,
    -30619834, -7948175,
    30787171, -10921385,
    -20939770, 20425014,
    9217270, -30851736,
    7143124, 27841882,
    -18617035, -23916065,
    28020613, 8976638,
    -27972236, 2419825,
    23717121, -18227761,
    -11416892, 24073306,
    -773934, -29176246,
    15106013, 22216947,
    -22950636, -14907895,
    27677100, -525622,
    -23143495, 10917008,
    15710949, -22958105,
    -2161121, 24988600,
    -9292814, -25225669,
    20718446, 15205249,
    -24618115, -5748672,
    24801196, -8961108,
    -16737893, 17276171,
    7060756, -25254470,
    6474771, 23168140,
    -15998247, -19334893,
    23798764, 7084768,
    -23425143, 2945425,
    19804375, -15799463,
    -9039594, 21178731,
    -1432526, -24779605,
    13763745, 19037191,
    -20381299, -11982254,
    24177583, -1069441,
    -20120699, 10510497,
    13357514, -21036890,
    -357943, 22971609,
    -9918221, -21431185,
    19167731, 12275825,
    -21900666, -3497663,
    21507254, -9041765,
    -13866286, 16426194,
    4986252, -22686482,
    7152013, 20527536,
    -15333455, -16192793,
    21827353, 5233356,
    -20914078, 4231856,
    16932502, -15314563,
    -6780393, 19828432,
    -2785586, -22186694,
    13816073, 16495673,
    -19174852, -9332218,
    21989145, -2504464,
    -17218244, 11239908,
    10438956, -19431092,
    859303, 20577654,
    -9882052, -19000807,
    18444280, 10455394,
    -20367731, -1738488,
    19397940, -9600970,
    -11596816, 16298618,
    3123180, -21130708,
    8145803, 18652562,
    -15270743, -13695044,
    20646776, 3405970,
    -18950128, 5632406,
    14593874, -15270490,
    -4697740, 19039789,
    -4227404, -20142112,
    14088453, 14350990,
    -18404237, -7051402,
    20274758, -3835734,
    -15131923, 11908990,
    8252192, -18849631,
    2589674, 19207768,
    -10642367, -16723080,
    18109515, 8388261,
    -19056391, 216611,
    17337743, -10437153,
    -9340373, 16305397,
    1180275, -19782725,
    9213758, 16814512,
    -15335553, -11347624,
    19610301, 1527199,
    -17117952, 7003816,
    12371279, -15392167,
    -2641861, 18301634,
    -5636899, -18299060,
    14492992, 12263164,
    -17706758, -4815466,
    18644913, -5273024,
    -12904916, 12582605,
    6083173, -18084613,
    4118188, 17800111,
    -11330642, -14550329,
    17703743, 6345392,
    -17677819, 2028755,
    15238759, -11110531,
    -7276900, 15958391,
    -215680, -18513388,
    10135936, 15483485,
    -15709674, -9276985,
    18798582, -453390,
    -15295280, 8467650,
    10122732, -15524760,
    -612905, 17546963,
    -7032816, -16470050,
    14865023, 10193923,
    -17038593, -2598039,
    16950502, -6675292,
    -10829629, 13152847,
    3967424, -17521641,
    5778380, 16382369,
    -12029784, -12343630,
    17293306, 4259858,
    -16300962, 3815799,
    13274145, -11897341,
    -5018490, 16058582,
    -2506898, -17088629,
    11119144, 13013954,
    -15178730, -6827677,
    17375266, -2095143,
    -13308512, 9407308,
    8002119, -15320640,
    1230102, 16548383,
    -8182182, -14535059,
    14936526, 8067287,
    -16056985, -590614,
    15150555, -7858173,
    -8632714, 13413177,
    1997843, -16576598,
    7114437, 14757364,
    -12383551, -10131827,
    16580584, 2274972,
    -14660668, 5340633,
    11159058, -12285728,
    -2976266, 15510906,
    -3972120, -15573091,
    11833759, 11100356,
    -14761968, -4573188,
    15932068, -3626291,
    -11345202, 10220829,
    5875645, -14982454,
    2976332, 15302338,
    -9006556, -12472783,
    14734528, 6050574,
    -14883537, 1246372,
    13287082, -8750797,
    -6530307, 13489082,
    119956, -15437602,
    8184519, 13021960,
    -12511998, -7982581,
    15685416, 415438,
    -12915315, 6671774,
    9008131, -12440977,
    -1011058, 14736133,
    -5291439, -13807255,
    12137233, 9056733,
    -14026896, -2537436,
    14377038, -4958328,
    -9228519, 10810540,
    3734833, -14243547,
    4487176, 13689651,
    -9299700, -10120552,
    13794040, 4418973,
    -13736405, 2372665,
    11663117, -9361836,
    -4707819, 13339133,
    -1466325, -14292777,
    8998937, 11307776,
    -12395421, -5975915,
    14559166, -1184659,
    -11201733, 7571415,
    7081001, -12362948,
    651768, 13811043,
    -6297214, -12131100,
    12226726, 7167521,
    -13175253, -769338,
    12786119, -6004803,
    -7362323, 11003170,
    2076276, -13485892,
    5567640, 12422440,
    -9890305, -8521293,
    13628861, 2365690,
    -12127956, 4106543,
    9548082, -9769403,
    -2670288, 12719274,
    -2726205, -12657433,
    9344401, 9512845,
    -11799864, -4155420,
    13203632, -2311340,
    -9487166, 7954674,
    5496076, -11758982,
    1718174, 12584037,
    -6443340, -10522514,
    11628682, 5939875,
    -12044318, 0
};
#define DEBUG_FFT_BYPASS_DIS    (1)
#if DEBUG_FFT_BYPASS_DIS == 1
int pre_proc_fft_ro[1024] =
{
    10906,  0,  -62320,  0,  116030,  0,  -24026,  0,
    -184090,  0,  196793,  0,  -2158,  0,  -54012,  0,
    756,  0,  7225,  0,  -63325,  0,  91934,  0,
    -6525,  0,  -73040,  0,  52421,  0,  46800,  0,
    -17563,  0,  -87584,  0,  28365,  0,  11210,  0,
    24864,  0,  -21070,  0,  105831,  0,  -177760,  0,
    31722,  0,  45448,  0,  83740,  0,  -89964,  0,
    14410,  0,  18032,  0,  -58938,  0,  27260,  0,
    12036,  0,  -28440,  0,  -32452,  0,  96875,  0,
    -14351,  0,  -61230,  0,  90684,  0,  -154035,  0,
    112332,  0,  84036,  0,  -30459,  0,  -64386,  0,
    -197276,  0,  454176,  0,  -422995,  0,  256434,  0,
    -87318,  0,  -8910,  0,  47040,  0,  35776,  0,
    -67200,  0,  -217160,  0,  328874,  0,  -53382,  0,
    18333,  0,  -137802,  0,  88059,  0,  -71556,  0,
    -67116,  0,  204880,  0,  -53000,  0,  -97200,  0,
    69300,  0,  62550,  0,  60227,  0,  -93899,  0,
    -54510,  0,  -32428,  0,  -29520,  0,  48750,  0,
    194820,  0,  -290339,  0,  241296,  0,  -114972,  0,
    -162162,  0,  135386,  0,  -121824,  0,  236488,  0,
    -219000,  0,  183249,  0,  -113854,  0,  -75215,  0,
    162864,  0,  117924,  0,  -194810,  0,  -12099,  0,
    -123172,  0,  90653,  0,  278388,  0,  -371984,  0,
    94957,  0,  156088,  0,  -13794,  0,  -293296,  0,
    315282,  0,  -306611,  0,  39936,  0,  123240,  0,
    447535,  0,  -426263,  0,  -70238,  0,  172628,  0,
    -375717,  0,  423000,  0,  -83032,  0,  180544,  0,
    -319880,  0,  -134835,  0,  521356,  0,  -83631,  0,
    -124740,  0,  -143676,  0,  25596,  0,  323804,  0,
    -570845,  0,  415386,  0,  15904,  0,  -89858,  0,
    23368,  0,  -229758,  0,  274560,  0,  -18900,  0,
    59472,  0,  -160026,  0,  -283446,  0,  276696,  0,
    367856,  0,  -248640,  0,  90560,  0,  -360932,  0,
    49708,  0,  324731,  0,  -206739,  0,  -234430,  0,
    511451,  0,  -378750,  0,  22032,  0,  484512,  0,
    -528528,  0,  184297,  0,  -471805,  0,  676896,  0,
    -434758,  0,  143440,  0,  -10528,  0,  401115,  0,
    -95667,  0,  -846450,  0,  724880,  0,  -7546,  0,
    -149256,  0,  29971,  0,  -136188,  0,  -234348,  0,
    169694,  0,  433557,  0,  250504,  0,  -996450,  0,
    -37485,  0,  396640,  0,  455060,  0,  -420560,  0,
    230580,  0,  -530417,  0,  535434,  0,  4632,  0,
    -233100,  0,  400384,  0,  -570575,  0,  34056,  0,
    192077,  0,  635184,  0,  -669810,  0,  291149,  0,
    -654432,  0,  546786,  0,  56168,  0,  -24930,  0,
    29225,  0,  -53760,  0,  -322408,  0,  362523,  0,
    90418,  0,  258258,  0,  -412036,  0,  91035,  0,
    -21775,  0,  -126000,  0,  72957,  0,  -135099,  0,
    426240,  0,  -540552,  0,  372736,  0,  -693000,  0,
    990591,  0,  -154190,  0,  -246881,  0,  -608475,  0,
    609297,  0,  -102342,  0,  404662,  0,  -544394,  0,
    187533,  0,  -65520,  0,  -370905,  0,  609178,  0,
    -12298,  0,  -304629,  0,  -580720,  0,  727710,  0,
    -93884,  0,  549692,  0,  -852176,  0,  335549,  0,
    -390910,  0,  118584,  0,  414375,  0,  -229595,  0,
    -178360,  0,  186580,  0,  191090,  0,  182595,  0,
    -509335,  0,  535140,  0,  -538206,  0,  60695,  0,
    133598,  0,  -258741,  0,  942942,  0,  -815439,  0,
    434732,  0,  -338016,  0,  359856,  0,  -278484,  0,
    601545,  0,  -93104,  0,  -650346,  0,  35490,  0,
    339010,  0,  116955,  0,  -119106,  0,  265698,  0,
    -695977,  0,  161160,  0,  -53092,  0,  419631,  0,
    -501802,  0,  279006,  0,  68541,  0,  -156519,  0,
    -96256,  0,  -353280,  0,  375808,  0,  77824,  0,
    -95232,  0,  -87040,  0,  -499712,  0,  612352,  0,
    344751,  0,  -53196,  0,  -617288,  0,  -1010758,  0,
    1747952,  0,  349182,  0,  -482460,  0,  -1218724,  0,
    427560,  0,  954884,  0,  26442,  0,  114695,  0,
    -1574742,  0,  1230795,  0,  -237820,  0,  1038297,  0,
    -624571,  0,  -675360,  0,  548270,  0,  -72288,  0,
    -13039,  0,  239239,  0,  -580419,  0,  584242,  0,
    -167160,  0,  -71496,  0,  869107,  0,  -1397457,  0,
    753081,  0,  -579180,  0,  96236,  0,  342020,  0,
    231549,  0,  -325650,  0,  253692,  0,  -401580,  0,
    165357,  0,  512848,  0,  -319052,  0,  -1262644,  0,
    1536595,  0,  -931056,  0,  689923,  0,  -502326,  0,
    -177284,  0,  508938,  0,  -359424,  0,  1027233,  0,
    -2044729,  0,  1088050,  0,  164116,  0,  130498,  0,
    -738405,  0,  674140,  0,  -746461,  0,  1408680,  0,
    -1403100,  0,  242816,  0,  298820,  0,  -132312,  0,
    502427,  0,  -967779,  0,  1325625,  0,  -874484,  0,
    -532338,  0,  479272,  0,  837408,  0,  -988627,  0,
    -438933,  0,  883668,  0,  -68880,  0,  -267200,  0,
    448740,  0,  -227150,  0,  -400648,  0,  606288,  0,
    -109485,  0,  -374448,  0,  781950,  0,  -800188,  0,
    202752,  0,  -124346,  0,  229126,  0,  602952,  0,
    -851516,  0,  491006,  0,  -66968,  0,  -41580,  0,
    124666,  0,  140994,  0,  -463240,  0,  27195,  0,
    -67160,  0,  514040,  0,  -312046,  0,  -36363,  0,
    361080,  0,  98280,  0,  -259284,  0,  -707584,  0,
    1046150,  0,  -576640,  0,  197100,  0,  -656958,  0,
    286416,  0,  832370,  0,  -386636,  0,  -624682,  0,
    336525,  0,  -183515,  0,  115736,  0,  245856,  0,
    243492,  0,  -649332,  0,  -227250,  0,  346777,  0,
    398055,  0,  126046,  0,  -780054,  0,  512108,  0,
    -453024,  0,  776552,  0,  -588560,  0,  356776,  0,
    36783,  0,  -354036,  0,  -164322,  0,  256473,  0,
    -162225,  0,  487240,  0,  -254944,  0,  -508,  0,
    -192768,  0,  260428,  0,  237644,  0,  -541260,  0,
    -9101,  0,  221832,  0,  -83304,  0,  252252,  0,
    -185542,  0,  -277816,  0,  483715,  0,  -156200,  0,
    -121086,  0,  -89880,  0,  554976,  0,  -710151,  0,
    346492,  0,  -3248,  0,  -44912,  0,  15800,  0,
    67080,  0,  -402432,  0,  471476,  0,  -32164,  0,
    -56672,  0,  -45738,  0,  -74822,  0,  -46949,  0,
    -112428,  0,  457596,  0,  -79869,  0,  -75032,  0,
    -198162,  0,  128800,  0,  -127434,  0,  215592,  0,
    -119730,  0,  53454,  0,  164241,  0,  -157096,  0,
    -160146,  0,  216858,  0,  -214616,  0,  283374,  0,
    -109344,  0,  -111936,  0,  -56462,  0,  21165,  0,
    253250,  0,  -123738,  0,  -215622,  0,  157605,  0,
    37979,  0,  26793,  0,  105975,  0,  -383020,  0,
    374112,  0,  -106424,  0,  84448,  0,  -194412,  0,
    148137,  0,  -240340,  0,  254567,  0,  -29106,  0,
    -185070,  0,  292110,  0,  -305448,  0,  361025,  0,
    -245272,  0,  81648,  0,  -20130,  0,  -116478,  0,
    172378,  0,  -30690,  0,  -118712,  0,  92231,  0,
    6716,  0,  -3432,  0,  3666,  0,  -69966,  0,
    35370,  0,  29304,  0,  -23790,  0,  72644,  0,
    -17250,  0,  -122366,  0,  59280,  0,  20768,  0,
    45936,  0,  -58824,  0,  15120,  0,  -40590,  0,
    27864,  0,  47594,  0,  16120,  0,  -105978,  0,
    -12322,  0,  58311,  0,  28126,  0,  -9024,  0,
    -61085,  0,  36363,  0,  -38732,  0,  71162,  0,
    -44730,  0,  8544,  0,  -22616,  0,  -5046,  0,
    91246,  0,  -111690,  0,  25160,  0,  15204,  0,
    15876,  0,  76775,  0,  -104746,  0,  -54940,  0,
    75112,  0,  60188,  0,  -8774,  0,  -34850,  0,
};
const int mfcc_out_ref[NUM_FBANK] =
{
    -1432,  85,  395,  526,  727,  725,  523,  357,
        349,  246,  -2,  115,  103,  -75,  -81,  131,
        87,  112,  17,  -57,  -284,  -165,  -32,  -56,
        154,  71,  1,  95,  146,  214,  266,  129,
        75,  7,  132,  127,  169,  208,  183,  458,
        475,  495,  588,  570,  628,  531,  530,  581,
        680,  652,  750,  656,  744,  823,  631,  1001,
        689,  1482,  1014,  1828,  1332,  935,  986,  1237
    };

#else
int pre_proc_fft_ro[1024] =
{
    70784,  0,  -3171,  -67941,  -7165,  99288,  10991,  -72808,
    124522,  94280,  -309749,  -62193,  516986,  -292409,  -346621,  378361,
    -582311,  -501420,  1423337,  811744,  -1140887,  -639201,  763942,  523705,
    -114743,  -746647,  87141,  954083,  -591749,  -537792,  646425,  136154,
    12117,  -143968,  -289383,  400216,  263026,  -394501,  83993,  426620,
    -244900,  -108766,  -59637,  -30948,  -196493,  -74045,  252418,  -59233,
    -107301,  826513,  154830,  -438429,  458660,  -67971,  -906486,  -116653,
    480713,  215559,  -79767,  -580531,  -143552,  411357,  404580,  387172,
    -212164,  -200086,  -20197,  41532,  -150770,  -109324,  -251578,  74569,
    -405043,  -358832,  824148,  340319,  -345437,  15687,  582580,  348340,
    39389,  272192,  -1675785,  418815,  2373289,  -1317329,  -890210,  1587766,
    -948130,  -1017178,  723222,  -840238,  493442,  503095,  -237752,  733690,
    -971924,  -525233,  1725358,  -150729,  -1731913,  794275,  1458358,  -1441922,
    -805188,  1967201,  50128,  -1213571,  -554601,  -1832778,  -1601363,  2319325,
    4243920,  1160476,  -2559260,  62636,  1978414,  -3012128,  -1535206,  2791935,
    -957543,  -520595,  -581799,  -1251148,  418604,  625739,  1133219,  1631413,
    -506346,  -1126358,  -877285,  722867,  399195,  -1435408,  893268,  529607,
    591778,  150883,  -713566,  204326,  -2193582,  1140114,  -2177556,  -323851,
    4468375,  1546968,  -1860561,  -843481,  1335523,  1310672,  -429227,  -359577,
    -116928,  1429598,  -2439989,  581806,  2508679,  -1462189,  3683337,  -2238236,
    -1322303,  3429159,  -2128606,  -1163461,  -2666156,  184398,  2306377,  -240807,
    -83277,  756727,  1098957,  171531,  -2839469,  -1838630,  3977761,  985489,
    -5580799,  3084331,  2798488,  -3131642,  -4301326,  -14722895,  3887927,  22644818,
    -392872,  -6524004,  -254860,  -5190090,  -788461,  1311896,  5385696,  1487882,
    -10051277,  215378,  7852548,  89126,  -5359766,  2528701,  9630130,  -4956316,
    -14467892,  9413497,  9352118,  -6234262,  -178001,  8052923,  -9234465,  -9063817,
    9868465,  4707240,  141738,  -16260555,  -5538303,  9203890,  1458922,  -6576074,
    2049475,  8906104,  1655786,  -10113126,  -9620865,  11397287,  -1472982,  -15886826,
    14591015,  8277924,  -10194964,  -5658105,  5339415,  4476759,  -5012589,  -3666740,
    8752917,  638496,  -7551714,  1959438,  4288085,  -6681388,  -7222942,  1848893,
    1729141,  917962,  5744042,  -3120015,  -329834,  7853555,  -10194720,  -5765192,
    6878392,  -2966599,  -210139,  6824436,  3505270,  -7724136,  -4795478,  6420186,
    -194097,  -10516078,  3331266,  4546864,  -2669085,  5451556,  -1010921,  -10361142,
    2743014,  7328550,  -4329238,  8903562,  -1044391,  -18887769,  4666547,  6844267,
    1572815,  5666400,  -4750793,  -6448559,  -2514447,  3825391,  5860430,  2510766,
    5255575,  -489419,  -7054550,  -4904152,  -3527352,  -2038906,  12905135,  15966378,
    -9626278,  -13278921,  5054816,  2217882,  -6115990,  -1973512,  -2718509,  1080769,
    2332516,  -1962548,  -1703993,  6808811,  3591245,  -5385486,  4369559,  -3091359,
    -8907878,  2537812,  2240164,  -3776208,  6376281,  -390148,  -10619582,  3638115,
    -2346972,  -5080896,  2799358,  7668648,  -6934200,  3643306,  6368574,  -5300988,
    4973969,  13284195,  -8376816,  -9491810,  7349817,  455668,  -4865328,  1024490,
    5449965,  1008400,  457367,  9067425,  -11238360,  -2591879,  7959768,  -15393944,
    -5521983,  7694985,  4627675,  7018662,  427741,  4155600,  -207443,  -6116356,
    -1173003,  5243176,  1379792,  -3183042,  3353509,  2194518,  -6698664,  5235839,
    3262093,  -5424818,  -1457863,  6210041,  3164449,  -7240952,  907159,  5948103,
    -8647003,  6608321,  5620838,  -7183676,  6298923,  3470034,  -9384854,  -2510272,
    3794216,  4376915,  8172378,  -2259831,  -7858389,  -25940,  -1403989,  1606023,
    15632412,  2810214,  -17779972,  3814505,  206393,  -5876017,  -2751472,  1161830,
    5129978,  -6863841,  6084945,  11021857,  -1698289,  -12779555,  10059691,  9908287,
    -9611254,  -2318961,  5847418,  -1701531,  -12931639,  836582,  -1773053,  -7396415,
    14679079,  14252862,  -8292260,  -5053652,  -1045549,  -11223504,  4010029,  3719362,
    -7660576,  7628226,  13833595,  -6943478,  -3493851,  6765293,  -5814665,  -6444782,
    14416161,  -1875759,  -8515722,  9913197,  3501592,  -9452847,  -10648400,  4546727,
    7896142,  -22617330,  -1637165,  28775324,  569186,  -10808677,  11952067,  4365394,
    -1720999,  -639077,  -7479599,  2695711,  844686,  5329927,  13640253,  -8028720,
    -11567809,  -3753565,  3838631,  1275222,  -1476240,  -6242355,  2069511,  -952724,
    -4460309,  4761436,  3114145,  13650130,  -5515768,  -17068139,  9306512,  24327238,
    -12561188,  -4728366,  12888331,  -11101302,  -2241326,  4533553,  -4973193,  -16920810,
    10615041,  11187951,  -8914938,  2889420,  2151562,  -4201595,  656808,  3834600,
    -1740008,  1279214,  -653924,  -6868379,  -3536820,  5077199,  -2889720,  -11943840,
    20453265,  23735447,  -13451937,  -39161235,  13298602,  15844591,  -3825912,  16588053,
    -16387229,  -18284737,  2152622,  10532826,  4096596,  -13125080,  4022133,  10226190,
    -8803658,  3638674,  2622754,  -5980077,  2196971,  3307057,  1308968,  -685094,
    -1608357,  -470380,  -224318,  19196,  -76163,  -443703,  1590301,  700944,
    -2326302,  0,
};
const int mfcc_out_ref[NUM_FBANK] =
{
    -1432,  85,  395,  526,  727,  725,  523,  357,
        349,  246,  -2,  115,  103,  -75,  -81,  131,
        87,  112,  17,  -57,  -284,  -165,  -32,  -56,
        154,  71,  1,  95,  146,  214,  266,  129,
        75,  7,  132,  127,  169,  208,  183,  458,
        475,  495,  588,  570,  628,  531,  530,  581,
        680,  652,  750,  656,  744,  823,  631,  1001,
        689,  1482,  1014,  1828,  1332,  935,  986,  1237
    };
#endif

#endif  //__IET_MFCCDATA_H__