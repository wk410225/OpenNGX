typedef struct {
    int USAGE_KEY; // int
    char SRV_ID[5]; // char(4)
    char PARAM_USAGE[2]; // char(1)
    char PARAM_INDEX[3]; // char(2)
    char PARAM_DATA[65]; // char(64)
    char PARAM_DSP[65]; // char(64)
} tbl_srv_param;

