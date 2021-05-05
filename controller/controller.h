// keys definition
#define KEY1_MASK 0x80  // keys on KEY1 pin
#define KEY2_MASK 0x40  // keys on KEY2 pin

#define KEY1_PLAYPAUSE  ((KEY1_MASK << 8) + 0)
#define KEY1_INT        ((KEY1_MASK << 8) + 1)
#define KEY1_RPT        ((KEY1_MASK << 8) + 2)
#define KEY1_RDM        ((KEY1_MASK << 8) + 3)
#define KEY1_STOP       ((KEY1_MASK << 8) + 4)
#define KEY1_OSD        ((KEY1_MASK << 8) + 5)
#define KEY1_BAND       ((KEY1_MASK << 8) + 7)
#define KEY1_SEEKUP     ((KEY1_MASK << 8) + 10)
#define KEY1_SEEKDOWN   ((KEY1_MASK << 8) + 12)
#define KEY1_ENC        ((KEY1_MASK << 8) + 16)
#define KEY1_AMS        ((KEY1_MASK << 8) + 30)

#define KEY2_MUTE       ((KEY2_MASK << 8) + 1)
#define KEY2_LOC        ((KEY2_MASK << 8) + 2)
#define KEY2_DISP       ((KEY2_MASK << 8) + 4)
#define KEY2_PWR1       ((KEY2_MASK << 8) + 5)
#define KEY2_PWR2       ((KEY2_MASK << 8) + 6)
#define KEY2_MOD        ((KEY2_MASK << 8) + 7)
#define KEY2_EQ         ((KEY2_MASK << 8) + 10)
#define KEY2_ST         ((KEY2_MASK << 8) + 14)
