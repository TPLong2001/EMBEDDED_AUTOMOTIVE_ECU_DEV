#include<stdint.h>

#define FLASH_BASE   0x40022000UL
#define FLASH       ((FLASH_Typedef*) FLASH_BASE)

typedef struct{
    volatile uint32_t FLASH_ACR;
    volatile uint32_t FLASH_KEYR;
    volatile uint32_t FLASH_OPTKEYR;
    volatile uint32_t FLASH_SR;
    volatile uint32_t FLASH_CR;
    volatile uint32_t FLASH_AR;
    uint32_t Reserved;
    volatile uint32_t FLASH_OBR;
    volatile uint32_t FLASH_WRPR;  
} FLASH_Typedef;


#define FLASH_CR_LOCK       (1U << 7)
#define FLASH_CR_STRS       (1U << 6)
#define FLASH_CR_PER        (1U << 1)
#define FLASH_CR_PG         (1U << 0)

#define FLASH_SR_EOP        (1U << 5)
#define FLASH_SR_BSY        (1U << 0)



#define FLASH_PAGE_SIZE     1024U
// #define FLASH_ADDR_TARGET   0x0800xxxx
#define FLASH_KEY_UNLOCK1   0x45670123
#define FLASH_KEY_UNLOCK2   0xCDEF89AB



// Mở Khóa Flash
//      Kiểm tra LOCK của FLASH_CR
//      Nếu đang khóa thì tiến hành mở khóa
//      ghi liên tiếp 2 giá trị vào FLASH_KEYR là 0x45670123 và 0xCDEF89AB
//      FLASH đã mởmở
void FLASH_Unlock(void){
    if(FLASH->FLASH_CR & FLASH_CR_LOCK){
        FLASH->FLASH_KEYR = FLASH_KEY_UNLOCK1;
        FLASH->FLASH_KEYR = FLASH_KEY_UNLOCK2;
    }
}


// Khóa FLASH
//      set LOCK của FLASH_CR lên 1
void FLASH_Lock(void){
    FLASH->FLASH_CR |= FLASH_CR_LOCK;
}


// Xóa 1 page 
//      Mở khóa FLASH
//      Đợi nếu Flash đang bận  - BSY của FLASH_SR
//      Chọn chế độ xóa         - PER của FLASH_CR
//      setup địa chỉ xóa       - FLASH_AR
//      bắt đầu xóa             - STRT của FLASH_CR
//      đợi xóa hoàn tất        - BSY của FLASH_SR và EOP của FLASH_SR
//      Tắt chế độ xóa          - PER của FLASH_CR
//      Khóa FlashFlash         - LOCK của FLASH_CR
void FLASH_ErasePage(uint32_t addr){
    FLASH_Unlock();

    while(FLASH->FLASH_SR & FLASH_SR_BSY);

    FLASH->FLASH_CR |= FLASH_CR_PER;
    FLASH->FLASH_AR = addr;
    
    FLASH->FLASH_CR |= FLASH_CR_STRS;

    while(FLASH->FLASH_SR & FLASH_SR_BSY);
    FLASH->FLASH_CR &= ~FLASH_CR_PER;

    FLASH_Lock();
}



// Ghi 1 haft-word (16 bit)
//      Mở khóa Flash           
//      Chọn chế độ ghi         - PG của FLASH_CR
//      Ghi data trực tiếp vào địa chỉ cần ghi
//      Đợi ghi hoàn tất        - BSY của FLASH_SR và EOP của FLASH_SR
//      Khóa FlashFlash         - LOCK của FLASH_CR
void FLASH_Write_16bit(uint32_t addr, uint16_t data){
    FLASH_Unlock();

    while(FLASH->FLASH_SR & FLASH_SR_BSY);

    FLASH->FLASH_CR |= FLASH_CR_PG;
    *(volatile uint16_t *)addr  = data;

    while(FLASH->FLASH_SR & FLASH_SR_BSY);

    FLASH_Lock();
}



// Đọc 1 haft-word
//      Đọc trực tiếp từ địa chỉ truyền vào
void FLASH_Read_16bit(uint32_t addr){
    return *(volatile uint16_t *) addr;
}







