#ifdef BLUENRG_3
#define NVM_BASE_ADDRESS (uint32_t)0x40001000
#else
#define NVM_BASE_ADDRESS (uint32_t)0x40100000
#endif

// Registers

#define NVM_REG_COMMAND    (NVM_BASE_ADDRESS + 0x00)  //0x00
#define NVM_REG_CONFIG     (NVM_BASE_ADDRESS + 0x04)  // 0x04
#define NVM_REG_IRQSTAT    (NVM_BASE_ADDRESS + 0x08)  // 0x08
#define NVM_REG_IRQMASK    (NVM_BASE_ADDRESS + 0x0C)  // 0x0C
#define NVM_REG_IRQRAW     (NVM_BASE_ADDRESS + 0x10)  // 0x10
#define NVM_REG_ADDRESS    (NVM_BASE_ADDRESS + 0x18)  // 0x18
#define NVM_REG_DATA       (NVM_BASE_ADDRESS + 0x40)  // 0x40
#define NVM_REG_DATA1      (NVM_BASE_ADDRESS + 0x44)  // 0x44
#define NVM_REG_DATA2      (NVM_BASE_ADDRESS + 0x48)  // 0x48
#define NVM_REG_DATA3      (NVM_BASE_ADDRESS + 0x4C)  // 0x4C

// Bit fields

#define NVM_IRQ_CMDDONE   0x01
#define NVM_IRQ_CMDBUSY   0x02  // CMDSTART
#define NVM_IRQ_CMDERR    0x04
#define NVM_IRQ_ILLCMD    0x08
#define NVM_IRQ_READOK    0x10
#define NVM_IRQ_FLNREADY  0x20

#define NVM_CMD_ERASE            0x11
#define NVM_CMD_MASSERASE        0x22
#define NVM_CMD_WRITE            0x33
#define NVM_CMD_BURSTWRITE       0xCC
