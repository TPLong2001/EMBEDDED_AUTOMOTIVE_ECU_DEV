# MEMORY -> FLASH

## Tổng Quan

__Bộ nhớ (memory)__ là nơi lưu trữ chương trình hoặc là nơi chứa các thông tin mà CPU đang làm việc.  

__Có 2 kiểu bộ nhớ cơ bản:__

- RAM (Random access memory) là bộ nhớ lưu các dữ liệu mà CPU đang làm việc. Dữ liệu trong RAM sẽ bị xóa khi mất điện.
- ROM/EPROM/EEPROM hoặc flash: là bộ nhớ lưu trữ chương trình vận hành của vi điều khiển, chúng được ghi khi vi điều khiển được nạp chương trình. Nội dung trong các loại bộ nhớ này không bị mất khi mất điện hoặc reset.

__So sánh bộ nhớ: RAM, FLASH, EPROM:__
| **RAM** | **FLASH**| **EPROM** |
|--------------|---------------|-------------|
| - Tốc độ đọc/ghi nhanh.  | - Tốc độ ghi chậm.      | - Tương tự FLASH, tuy nhiên có thể đọc/ghi theo từng byte.  |
| - Dữ liệu bị mất khi **ngưng** cấp nguồn. | - Tốc độ đọc nhanh|   |
|  | - Dữ liệu không bị mất khi **ngưng** cấp điện.|    |
|   | - **Phải xóa Flash** trước khi tiến hành ghi lại dữ liệu. Không thể xóa từng byte mà phải xóa theo **Page** (1 vùng nhớ được phân chia rõ ràng). |  |
|  | - Chỉ có thể đọc hoặc ghi theo **khối 2/4 byte**. |  |


Trong kiếm trúc __ARM Cortex-M__, __512MB đầu tiên__ trong không gian địa chỉ 4GB (từ 0x0000_0000 đến 0x1FFF_FFFF) được __dành cho bộ nhớ chương trình (code)__. 

Tiếp theo đó là:
- __512 MB SRAM__ (RAM nội) => (từ `0x2000_0000` đến `0x3FFF_FFFF`)
- __512 MB Peripheral__ (Thanh ghi điều khiển ngoại vi). => (từ `0x4000_0000` đến `0x5FFF_FFFF`)
- __512 MB Extern RAM__ (RAM ngoài). => (từ `0x6000_0000` đến `0x9FFF_FFFF`).


Trong đó __không gian địa chỉ__ dành cho __bộ nhớ code__ từ __0x0000_0000 -> 0x1FFF_FFFF__ là vùng dùng để lưu __chương trình, firmware, vectorTable, ROM hệ thống__,...

__Nó bao gồm:__

- __Alias vùng boot__: Bắt đầu từ `0x0000_0000`.
- __Main Flash Memory (Flash)__: Bắt đầu từ __`0x8000_0000`__.
- __System Memory (ROM Bootloader)__: Bắt đầu từ __`0x1FFF_0000`__.
- __Option Bytes__: Bắt đầu từ __`0x1FFF_F000`__.
- __OTP (One-Time Programmable)__: Bắt đầu từ __`0x1FFF_7800`__.
- __Device ID / Flash size / UID__: Bắt đầu từ __`0x1FFF_F800`__.

### Vùng boot alias -  `0x0000_0000`

- Đây là vùng khởi động mặc định của CPU sau khi reset.

- Được ánh xạ (alias) tới 1 trong 3 vùng:

    - __BOOT0 = 0__ -> Ánh xạ tới `0x0800_0000` (__Flash__): chạy chương trình người dùng.
    - __BOOT0 = 1__ -> Ánh xạ tới `0x1FFF_0000` (__System ROM__): chạy bootloader của STM.
    - __BOOT0 = 1 + config__ -> Ánh xạ tới `0x2000_0000` (__SRAM__): chạy code từ RAM (__debug,...__).

### Vùng FLASH - `0x0 800_0000`

- Đây là vùng chứa firmware của người dùng.
- Kích thước tùy dòng: 64KB - 128kb - 1MB - ...
- Lưu trữ: 
    - `vector table` - chứa địa chỉ `msb`, hàm `resert_handler` và toàn bộ các __hàm xử lý ngắt (ISR)__ khác.
    - `.text` – _toàn bộ mã lệnh chương trình_.
    - `.rodata` – _hằng số_.
    - `.data` - _dữ liệu static/ global_ __có giá trị__. khi MCU khởi động sẽ được copy lên RAM. 


### Vùng System Memory (ROM Bootloader) - `0x1FFF_0000`

- Đây là bộ nhớ ROM do STMicroelectronics ghi sẵn.

- Không thể xóa hoặc thay đổi.

- Chứa chương trình Bootloader của ST: cho phép nạp firmware qua UART, USB DFU, CAN, SPI...

- Có ích khi:

    - Lập trình lần đầu

    - Cập nhật firmware (nếu BOOT0=1)

    - Không cần ST-Link để nạp


### Vùng Option Bytes - `0x1FFF_x000`

- Là vùng chứa cấu hình bảo vệ của Flash:

    - __RDP (Read Protection)__: Chống đọc trộm firmware

    - __WRP (Write Protection)__: Chống ghi/xóa sector

    - __USER options__: watchdog HW/SW, BOOT1 config, BOR...

- Truy cập thông qua thanh ghi `FLASH_OBR`, `FLASH_OPTCR`, không đọc trực tiếp như RAM.


### Vùng OTP (One-Time Programmable) ~ `0x1FFF_7800`

Dữ liệu chỉ được __ghi 1 lần duy nhất__ và __không thể ghi lại sau khi đã ghi__, dùng cho:

- Serial number

- Thông số hiệu chuẩn (ADC, nội RC, LSI...)

- Mã hóa, mã vùng

- Bảo mật hoặc hiệu chuẩn nhà máy

### Vùng Unique ID, Flash size, Device ID - `0x1FFF_F800` 

- Ghi các thông tin định danh chip, Flash size
- Kiểm tra mã hóa license theo chip
- Ghi log
- Kiểm tra dòng chip khi nạp firmware

---
## FLASH

Bộ nhớ __Flash__ trong STM32 là loại bộ nhớ không mất dữ liệu khi mất nguồn, đây là một dạng __EEPROM (Electrically Erasable Programmable Read-Only Memory)__ với hiệu năng cao, thường được tích hợp sẵn trong chip vi điều khiển dùng để:
- Lưu trữ chương trình (__firmware__) và dữ liệu cố định.
- Có thể __xóa/ghi lại được nhiều lần__ trong quá trình phát triển và sử dụng.
- __Giới hạn số chu kỳ ghi/xóa__ và thời gian giữa các lần: Mỗi sector Flash có tuổi thọ nhất định (thường tối thiểu 10k chu kỳ).

So với RAM, bộ nhớ Flash chậm hơn và có chu kỳ ghi/đọc giới hạn nhưng __không mất dữ liệu__. 

### Cấu tạo

Bộ nhớ Flash được kết nối với bus hệ thống của vi điều khiển. Trên STM32:
- Flash được nối vào bus I-Code/D-Code của CPU để tối ưu tốc độ truy xuất.
- Tích hợp bộ điều khiển Flash (Flash interface hoặc FPEC - Flash Program/Erase Controller) để quản lý việc đọc/ghi/xóa Flash bằng các thanh ghi chuyên dụng (`FLASH_ACR`, `FLASH_KEYR`, `FLASH_SR`, `FLASH_CR`).

- Phần cứng Flash bao gồm mảng các ô nhớ Flash (floating-gate transistor) được tổ chức thành các khối/sector, và bộ điều khiển Flash tích hợp để đảm bảo việc đọc/ghi an toàn, hiệu quả.

### Nguyên lý hoạt động

Dữ liệu trong Flash mặc định ở trạng thái `0xFF` (tất cả bit ‘1’ logic) sau khi xóa, và chuyển dần sang ‘0’ khi được lập trình.

Lưu ý rằng Flash chỉ cho phép đổi bit từ ‘1’ thành ‘0’ khi ghi, do đó nếu muốn sửa đổi từ ‘0’ thành ‘1’ ở bất kỳ bit nào, bắt buộc phải thực hiện xóa toàn bộ đơn vị nhớ chứa bit đó trước sau đó ghi lại từ ‘1’ thành ‘0’.

- __Đọc (Read)__: Tương tự như đọc từ SRAM, CPU truy cập FLASH qua bus hệ thống -> lấy dữ liệu tại địa chỉ yêu cầu.



- __Ghi (Program)__: 
    - Thực hiện các thao tác trước khi ghi/xóa Flash (vô hiệu hóa ngắt, ngưng các hoạt động với Flash,...)
    - Trước khi ghi phải __mở khóa__ bộ điều khiển Flash (ghi 1 cặp khóa đặc biệt vào  `FLASH_KEYR`).
    - Thực hiện cơ chế xóa vùng cần ghi. 
    - CPU bắt đầu ghi (đưa bit từ 1 xuống 0 ở các ô nhớ mong muốn). Ghi từng word (64-bit) hoặc half-word (32-bit). Cấu hình trong FLASH_CR.
    - Kiểm tra `BSY` của `FLASH_SR` và cờ `EOP` (End of Operation) để xác định hoàn tất.
    - Khóa lại bộ nhớ Flash để tránh vô tình ghi/xóa nhầm trong quá trình thiết bị chạy bình thường.


- __Xóa (Erase)__:
    - mở khóa Flash (tương tự như ghi).
    - chọn sector hoặc page cần xóa
    - Bắt đầu xóa các sector. (bit `STRT` trong `FLASH_CR` được set)
    - Đợi hoàn thành.



### Cơ Chế Bảo Vệ:

- __Read Protection (RDP)__ – _Bảo vệ chống đọc_: là cơ chế khóa nội dung Flash không cho phép đọc hoặc trích xuất bởi ngoại vi debug hoặc bootloader khi được kích hoạt. STM32 hỗ trợ __3 mức RDP__. Cấu hình `RDP` được lưu trong Option bytes (thường là byte đầu tiên). Giá trị `0xAA` tương ứng __Level 0__, giá trị khác (trừ `0xCC`) là __Level 1__, và `0xCC` tương ứng __Level 2__.
    - __Level 0__: Không bảo vệ. Mặc định chip ở mức 0, nội dung Flash có thể được đọc/ghi tự do qua debug hoặc bootloader (nếu không có bảo vệ khác).
    - __Level 1__: Bảo vệ chống đọc kích hoạt. Ở mức này, mọi truy cập đọc Flash từ bên ngoài (như qua cổng JTAG/SWD hoặc bootloader USART) đều __bị chặn__. Khi debug kết nối hoặc boot từ RAM, nếu cố đọc vùng Flash sẽ nhận lỗi (bus error). Nếu __hạ RDP__ từ __Level 1 về Level 0__ bằng cách ghi option byte tương ứng, toàn bộ Flash người dùng sẽ __bị xóa__ sạch (mass erase) trước khi trở lại Level 0.
    - __Level 2__: Khóa chip hoàn toàn. Mức này nâng bảo vệ lên cao nhất: __vô hiệu hóa hoàn toàn__ cổng debug (JTAG/SWD) và bootloader hệ thống, ngăn mọi cách truy cập từ bên ngoài​. Ở Level 2, chip hoạt động độc lập và __không thể trở về__ Level 1 hay 0 (không thể gỡ bảo vệ).

- __Write Protection (WRP)__ – _Bảo vệ chống ghi/xóa_: Đây là cơ chế ngăn chặn việc ghi đè hoặc xóa ở những sector/trang nhất định của Flash. Khác với khóa tạm thời bằng FLASH_CR, các bit WRP được lưu trong __Option bytes__ và __cố định cho đến khi được thay đổi__ (yêu cầu xóa và ghi lại option bytes).

## Demo

__Ví dụ:__
```c
#include<stdint.h>

#define FLASH_BASE   0x40022000UL
#define FLASH       ((FLASH_Typedef*) FLASH_BASE)

typedef struct{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    uint32_t Reserved;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;  
} FLASH_Typedef;


#define FLASH_CR_LOCK       (1U << 7)
#define FLASH_CR_STRS       (1U << 6)
#define FLASH_CR_PER        (1U << 1)
#define FLASH_CR_PG         (1U << 0)

#define FLASH_SR_BSY        (1U << 0)
#define FLASH_SR_PGERR      (1U << 2)
#define FLASH_SR_WRPRTERR   (1U << 4)
#define FLASH_SR_EOP        (1U << 5)


#define FLASH_PAGE_SIZE     1024U
#define FLASH_ADDR_TARGET   0x08000000
#define FLASH_OFFSET_SHIFT  0x00040000
#define FLASH_KEY_UNLOCK1   0x45670123
#define FLASH_KEY_UNLOCK2   0xCDEF89AB

inline uint32_t FLASH_Align_Addr_Page(uint32_t addr){
    return (addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
}


// Mở Khóa Flash
//      Kiểm tra LOCK của FLASH_CR
//      Nếu đang khóa thì tiến hành mở khóa
//      ghi liên tiếp 2 giá trị vào FLASH_KEYR là 0x45670123 và 0xCDEF89AB
//      FLASH đã mởmở
void FLASH_Unlock(void){
    if(FLASH->CR & FLASH_CR_LOCK){
        FLASH->KEYR = FLASH_KEY_UNLOCK1;
        FLASH->KEYR = FLASH_KEY_UNLOCK2;
    }
}


// Khóa FLASH
//      set LOCK của FLASH_CR lên 1
void FLASH_Lock(void){
    FLASH->CR |= FLASH_CR_LOCK;
}


// Xóa 1 page 
//      Mở khóa FLASH
//      Đợi nếu Flash đang bận  - BSY của FLASH_SR
//      Chọn chế độ xóa         - PER của FLASH_CR
//      setup địa chỉ xóa       - FLASH_AR
//      bắt đầu xóa             - STRT của FLASH_CR
//      đợi xóa hoàn tất        - BSY của FLASH_SR và EOP của FLASH_SR
//      Tắt chế độ xóa          - PER của FLASH_CR
//      Xóa cờ lỗi         - WRPRTERR và PGERR của FLASH_SR
//      Khóa FlashFlash         - LOCK của FLASH_CR
void FLASH_ErasePage(uint32_t addr, uint32_t numPage){
    uint32_t addr_Align = FLASH_Align_Addr_Page(addr);
    
    FLASH_Unlock();

    for(uint32_t i = 0; i < numPage; i++){
        uint32_t pageAddr = addr_Align + (i * FLASH_PAGE_SIZE);

        uint32_t timeout = 1000000;
        while((FLASH->SR & FLASH_SR_BSY) && timeout--);

        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = pageAddr;
        
        FLASH->CR |= FLASH_CR_STRS;

        timeout = 1000000;
        while((FLASH->SR & FLASH_SR_BSY) && timeout--);

        FLASH->CR &= ~FLASH_CR_PER;

        if (FLASH->SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR)){
            FLASH->SR |= FLASH_SR_WRPRTERR | FLASH_SR_PGERR;
        }
    }
    

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

    uint32_t timeout = 1000000;
    while((FLASH->SR & FLASH_SR_BSY) && timeout--);

    FLASH->CR |= FLASH_CR_PG;
    *(volatile uint16_t *)addr  = data;

    timeout = 1000000;
    while((FLASH->SR & FLASH_SR_BSY) && timeout--);

    if (FLASH->SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR)){
        FLASH->SR |= FLASH_SR_WRPRTERR | FLASH_SR_PGERR;
    }

    FLASH->CR &= ~FLASH_CR_PG;

    FLASH_Lock();
}



// Đọc 1 haft-word
//      Đọc trực tiếp từ địa chỉ truyền vào
void FLASH_Read_16bit(uint32_t addr){
    return *(volatile uint16_t *) addr;
}



void FLASH_EraseAndWrite(uint32_t startAddr, uint32_t *data, uint32_t length){
    // align
    uint32_t addr_Align = FLASH_Align_Addr_Page(startAddr);

    uint32_t endAddr = addr_Align + length * 4;

    // số page cần xóa
    uint32_t numPage = ((endAddr - addr_Align) +  FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    FLASH_Unlock();

    // Erase
    for(uint32_t i = 0; i < numPage; i++){
        uint32_t pageAddr = addr_Align + (i * FLASH_PAGE_SIZE);

        uint32_t timeout = 1000000;
        while((FLASH->SR & FLASH_SR_BSY) && timeout--);

        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = pageAddr;
        
        FLASH->CR |= FLASH_CR_STRS;

        timeout = 1000000;
        while((FLASH->SR & FLASH_SR_BSY) && timeout--);

        FLASH->CR &= ~FLASH_CR_PER;

        if (FLASH->SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR)){
            FLASH->SR |= FLASH_SR_WRPRTERR | FLASH_SR_PGERR;
        }
    }

    // Ghi
    for (uint32_t i = 0; i < length; i++) {
        uint32_t addr = addr_Align + i * 4;

        while (FLASH->SR & FLASH_SR_BSY); 

        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint16_t *)addr = (uint16_t)(data[i] & 0xFFFF);
        while (FLASH->SR & FLASH_SR_BSY);

        *(volatile uint16_t *)(addr + 2) = (uint16_t)((data[i] >> 16) & 0xFFFF);
        while (FLASH->SR & FLASH_SR_BSY);

        FLASH->CR &= ~FLASH_CR_PG;
    }


    FLASH_Lock();
}



```


