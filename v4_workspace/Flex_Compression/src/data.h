//Data provided by NCAR/EOL under sponsorship of the National Science Foundation. http://data.eol.ucar.edu/



#ifndef SRC_DATA_H_
#define SRC_DATA_H_

#include "rail_types.h"

#define DATA_SAMPLES 2048
#define DATA_TYPE uint8_t


struct __attribute__((__packed__)) data_row{
    uint16_t POSMVYear;
    uint8_t POSMVMonth;
    uint8_t POSMVDay;
    uint32_t POSMVTime;
    uint8_t POSMVHeadingAccuracy;
    uint16_t POSMVHeading;
    uint32_t POSMVLAT;
    uint32_t POSMVLON;
    uint8_t RMYHumidity;
    uint32_t RMYBaro;
    uint16_t TSGFSST;
    int16_t TSGFIntTemp;
    uint16_t TSGFCond;
    uint16_t TSGFSal;
} typedef data_row_t;
data_row_t data[64] = {
    {2007,4,14,1824560004,11,3171,5811965,16984041,89,99432,1306,-986,2563,3151},
    {2007,4,14,1825560038,11,3170,5812139,16984398,90,99424,1317,-978,2563,3151},
    {2007,4,14,1826560023,11,3165,5812310,16984762,90,99424,1323,-975,2564,3152},
    {2007,4,14,1827560007,11,3149,5812480,16985124,89,99424,1334,-967,2565,3152},
    {2007,4,14,1828560041,11,3168,5812635,16985467,89,99424,1344,-967,2566,3153},
    {2007,4,14,1829560025,11,3167,5812787,16985792,89,99416,1355,-959,2566,3153},
    {2007,4,14,1830560010,11,3166,5812937,16986122,89,99416,1364,-955,2567,3153},
    {2007,4,14,1831560044,11,3168,5813091,16986461,89,99416,1374,-956,2567,3154},
    {2007,4,14,1832560028,11,3170,5813256,16986816,89,99416,1390,-949,2568,3154},
    {2007,4,14,1833560012,11,3171,5813429,16987183,88,99424,1395,-947,2568,3154},
    {2007,4,14,1834560047,11,3170,5813607,16987557,88,99416,1410,-943,2569,3155},
    {2007,4,14,1835560031,11,3174,5813787,16987932,88,99424,1411,-940,2569,3155},
    {2007,4,14,1836560015,11,3166,5813958,16988298,88,99424,1421,-941,2569,3155},
    {2007,4,14,1837560049,11,3169,5814129,16988665,88,99424,1414,-935,2569,3154},
    {2007,4,14,1838560034,11,3165,5814306,16989040,88,99416,1424,-937,2569,3155},
    {2007,4,14,1839560018,11,3172,5814477,16989404,87,99432,1429,-938,2569,3155},
    {2007,4,14,1840560002,11,3174,5814629,16989728,88,99439,1431,-942,2569,3155},
    {2007,4,14,1841560037,11,3172,5814765,16990024,88,99439,1428,-943,2569,3155},
    {2007,4,14,1842560021,11,3198,5814875,16990273,87,99447,1424,-948,2569,3155},
    {2007,4,14,1843560005,11,3331,5814957,16990440,87,99455,1430,-954,2568,3155},
    {2007,4,14,1844560039,11,3356,5815018,16990554,87,99455,1427,-957,2568,3155},
    {2007,4,14,1845560024,11,3380,5815049,16990636,87,99455,1421,-958,2567,3154},
    {2007,4,14,1846560008,11,3335,5815060,16990709,87,99447,1428,-971,2567,3155},
    {2007,4,14,1847560042,11,3229,5815061,16990786,87,99447,1428,-975,2566,3154},
    {2007,4,14,1848560026,11,3107,5815054,16990870,87,99447,1430,-981,2565,3154},
    {2007,4,14,1849560011,11,3032,5815041,16990957,87,99447,1426,-983,2565,3153},
    {2007,4,14,1850560045,11,2987,5815023,16991035,86,99455,1425,-990,2564,3153},
    {2007,4,14,1851560029,11,2968,5815003,16991111,87,99455,1427,-997,2563,3153},
    {2007,4,14,1852560013,11,2954,5814986,16991199,87,99455,1418,-1003,2563,3153},
    {2007,4,14,1853560048,11,2963,5814968,16991288,87,99447,1421,-1007,2562,3153},
    {2007,4,14,1854560032,11,2954,5814946,16991365,86,99439,1426,-1005,2562,3152},
    {2007,4,14,1855560016,11,2946,5814922,16991439,86,99455,1417,-1013,2562,3153},
    {2007,4,14,1856560001,11,2969,5814898,16991507,86,99455,1424,-1014,2561,3152},
    {2007,4,14,1857560035,11,2922,5814867,16991558,86,99455,1422,-1015,2561,3152},
    {2007,4,14,1858560019,11,2854,5814834,16991610,86,99455,1428,-1018,2561,3152},
    {2007,4,14,1859560003,11,2841,5814804,16991663,86,99455,1425,-1019,2561,3152},
    {2007,4,14,1900560038,11,2846,5814774,16991714,86,99447,1428,-1023,2560,3152},
    {2007,4,14,1901560022,11,2852,5814743,16991767,86,99447,1426,-1022,2560,3151},
    {2007,4,14,1902560006,11,2862,5814714,16991821,86,99447,1424,-1028,2560,3152},
    {2007,4,14,1903560040,11,2870,5814686,16991875,86,99447,1427,-1028,2560,3152},
    {2007,4,14,1904560025,11,2891,5814658,16991931,86,99447,1418,-1026,2560,3151},
    {2007,4,14,1905560009,11,2913,5814631,16991986,86,99439,1423,-1027,2560,3151},
    {2007,4,14,1906560043,11,2929,5814606,16992043,86,99447,1429,-1030,2560,3152},
    {2007,4,14,1907560028,11,2953,5814581,16992100,86,99432,1427,-1026,2560,3151},
    {2007,4,14,1908560012,11,2969,5814556,16992159,86,99447,1427,-1031,2560,3152},
    {2007,4,14,1909560046,11,2987,5814534,16992217,86,99447,1430,-1031,2560,3152},
    {2007,4,14,1910560030,11,2978,5814510,16992276,85,99439,1423,-1029,2560,3152},
    {2007,4,14,1911560015,11,2962,5814486,16992338,86,99439,1433,-1031,2560,3152},
    {2007,4,14,1912560049,11,2955,5814461,16992399,86,99432,1427,-1030,2560,3152},
    {2007,4,14,1913560033,11,2950,5814437,16992462,85,99447,1432,-1029,2560,3152},
    {2007,4,14,1914560018,11,2935,5814412,16992525,85,99447,1432,-1031,2560,3152},
    {2007,4,14,1915560002,11,2928,5814388,16992590,86,99447,1436,-1028,2560,3151},
    {2007,4,14,1916560036,11,2925,5814364,16992655,86,99439,1431,-1033,2559,3152},
    {2007,4,14,1917560020,11,2920,5814339,16992719,85,99439,1430,-1033,2559,3151},
    {2007,4,14,1918560005,11,2914,5814314,16992784,85,99455,1433,-1033,2559,3152},
    {2007,4,14,1919560039,11,2908,5814289,16992849,85,99447,1434,-1030,2559,3151},
    {2007,4,14,1920560023,11,2897,5814263,16992917,85,99439,1424,-1033,2560,3152},
    {2007,4,14,1921560008,11,2890,5814238,16992985,85,99447,1436,-1033,2559,3151},
    {2007,4,14,1922560042,11,2899,5814214,16993054,85,99447,1434,-1033,2559,3152},
    {2007,4,14,1923560026,11,2911,5814191,16993123,85,99447,1436,-1035,2560,3152},
    {2007,4,14,1924560010,11,2930,5814168,16993193,86,99447,1437,-1033,2559,3151},
    {2007,4,14,1925560045,11,2932,5814145,16993263,85,99447,1433,-1035,2559,3151},
    {2007,4,14,1926560029,11,2928,5814121,16993335,86,99447,1437,-1036,2559,3151},
    {2007,4,14,1927560013,11,2943,5814099,16993406,85,99439,1444,-1038,2559,3152}
};



#define	MAX_COMPRESSED_LENGTH 700

#define DATA_LENGTH (DATA_SAMPLES * sizeof(DATA_TYPE))

#ifdef ENCODE

#define ENCODED_DATA_TYPE  int8_t

#define ENCODED_DATA_LENGTH 700


/* transform data into an easier to compress format */
int preprocess(DATA_TYPE * input, uint16_t input_length, int8_t* output)
{
	uint8_t* r_output = (uint8_t*) output;
	r_output[0] = input[0];
	for(int i = 1; i < input_length; i++){
		r_output[i] = input[i] - input[i-1];
	}
	return input_length;
}
/* transform data back from easier to compress format */
int postprocess(int8_t* input, int input_length, DATA_TYPE* output)
{
	uint8_t* r_input = (uint8_t*) input;
	output[0] = r_input[0];
	for(int i = 1; i < input_length; i++){
		output[i] = output[i-1] + r_input[i];
	}
	return input_length;
}

#endif


#endif /* SRC_DATA_H_ */
