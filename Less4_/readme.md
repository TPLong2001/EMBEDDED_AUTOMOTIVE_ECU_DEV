
---

# BÀI 4: Các chuẩn giao tiếp cơ bản

Các MCU truyền nhận dữ liệu với nhau hoặc với các thiết bị thông qua tín hiệu điện áp. Nhưng có 1 số vấn đề đồng bộ hóa (Synchronization).  

__Khi truyền dữ liệu số (digital), bên nhận cần biết:__

- Khi nào 1 bit bắt đầu và kết thúc?
- Đâu là bit đầu tiên của một byte (hoặc frame)?
- Làm sao phân biệt các bit khi chuỗi quá nhiều 0 hoặc 1 liên tiếp?

__Nếu không có chuẩn giao tiếp nào thì:__

- Không có sự đồng bộ giữa bên phát và bên nhận.
- Dữ liệu có thể bị lệch, sai hoặc mất.
- Các chuỗi như 00000000 hoặc 11111111 khiến bên nhận không biết là đang giữ nguyên bit hay đã chuyển sang bit mới.

Vì vậy các __chuẩn Giao tiếp__ ra đời như: __I2C, SPI, UART, …__


## SPI

### SPI là gì?

__SPI (Serial Peripheral Interface)__ là một chuẩn giao tiếp nối tiếp đồng bộ dùng để truyền dữ liệu giữa vi điều khiển (MCU) và các thiết bị ngoại vi như: cảm biến, bộ nhớ Flash, màn hình, ADC/DAC, ...  

SPI hoạt động dựa trên __xung clock__ do Master tạo ra để đồng bộ hóa dữ liệu trong quá trình truyền nhận.

### Quan hệ giao tiếp:

SPI sử dụng mô hình __Master–Slave__ với Một Master - Nhiều Slave
- Mỗi __Slave__ có một chân __CS__ riêng để được Master kích hoạt khi cần.
- __Master__ điều khiển toàn bộ quá trình truyền nhận:
    - Tạo xung đồng hồ (SCLK)
    - Chủ động chọn thiết bị Slave qua chân SS (Slave Select)
- __Slave__ là thiết bị bị động, chỉ truyền dữ liệu khi được Master chọn (CS = LOW).

## Các đường tín hiệu:

- __SCLK (Serial Clock):__ Là xung đồng hồ do Master tạo ra.  
- __MOSI (Master Out – Slave In):__ Dây truyền dữ liệu từ Master đến Slave.  
- __MISO (Master In – Slave Out):__ Dây truyền dữ liệu từ Slave về Master.
- __SS (Slave Select):__ Dùng để chọn thiết bị Slave cần giao tiếp.

### Chế độ truyền và tốc độ truyền
- SPI hoạt động ở chế độ __Full-duplex__.

- SPI có thể hoạt động ở __tốc độ cao__ từ vài kHz đến tens of MHz (thường lên tới 50 MHz hoặc hơn tùy MCU và thiết bị).  

### Khoảng cách truyền
- SPI chỉ truyền gần với khoảng cách tối ưu: __dưới 1 mét__.  

- Khi tốc độ cao (nhiều MHz), dây dài sẽ gây nhiễu, suy hao và sai dữ liệu.

### Truyền dữ liệu

SPI truyền dữ liệu __theo bit đồng bộ với xung nhịp SCLK__.

__Cách thức truyền:__
 - Master __kéo chân SS/CS__ của một Slave __xuống mức thấp__ (LOW) để bắt đầu giao tiếp.

- Master phát ra xung __clock__ (SCLK) để __đồng bộ__ việc truyền dữ liệu.

- Mỗi __chu kỳ xung = 1 bit dữ liệu__ được truyền đi và nhận về qua MOSI và MISO.

- Dữ liệu được ghi nhận tại __cạnh lên__ hoặc __cạnh xuống__ của xung clock, tùy thuộc vào __chế độ SPI__ (SPI Mode 0, 1, 2, 3).

- Cả Master và Slave đều __lưu__ các bit nhận được lại.

- Sau khi truyền đủ số bit (thường là 8, 16, hoặc 32), Master __kéo CS lên mức cao__ (HIGH) để kết thúc phiên giao tiếp.

__Chế độ SPI (SPI Mode 0, 1, 2, 3):__  


- __CPOL (Clock Polarity)__: Xác định mức logic của tín hiệu SCK khi không hoạt động.  
    - __CPOL = 0__: SCK ở __mức thấp__ khi không hoạt động.
    - __CPOL = 1__: SCK ở __mức cao__ khi không hoạt động.
- __CPHA (Clock Phase)__: Xác định cạnh của xung nhịp SCK mà dữ liệu được lấy mẫu.
    - __CPHA = 0__: Dữ liệu được lấy mẫu ở __cạnh thứ nhất__, cạnh lên (rising edge) nếu CPOL = 0, hoặc cạnh xuống (falling edge) nếu CPOL = 1.
    - __CPHA = 1__: Dữ liệu được lấy mẫu ở __cạnh thứ hai__, cạnh xuống (falling edge) nếu CPOL = 0, hoặc cạnh lên (rising edge) nếu CPOL = 1.


__SPI có 4 chế độ truyền, được xác định bởi CPOL (Clock Polarity) và CPHA (Clock Phase):__

__CPOL = 0, CPHA = 0 (Chế độ 0)__

- Dữ liệu được truyền tại SCK = 1.
- Slave nhận dữ liệu tại xung SCK lên (clock rising edge).
- Slave truyền dữ liệu tại xung SCK xuống (clock falling edge).
________________________________________
__CPOL = 0, CPHA = 1 (Chế độ 1)__

- Dữ liệu được truyền tại SCK = 0.
- Slave nhận dữ liệu tại xung SCK xuống (falling edge).
- Slave truyền dữ liệu tại xung SCK lên (rising edge).
________________________________________
__CPOL = 1, CPHA = 0 (Chế độ 2)__

- Dữ liệu được truyền tại SCK = 0.
- Slave nhận dữ liệu tại xung SCK xuống (falling edge).
- Slave truyền dữ liệu tại xung SCK lên (rising edge).
________________________________________
__CPOL = 1, CPHA = 1 (Chế độ 3)__

- Dữ liệu được truyền tại SCK = 1.
- Slave nhận dữ liệu tại xung SCK lên (rising edge).
- Slave truyền dữ liệu tại xung SCK xuống (falling edge).


---
## I2C
### I2C là gì?
__I2C (Inter-Integrated Circuit)__ là một chuẩn giao tiếp nối tiếp đồng bộ, được sử dụng rộng rãi để truyền dữ liệu giữa vi điều khiển (MCU) và các thiết bị ngoại vi như cảm biến, EEPROM, RTC, màn hình, ...

I2C chỉ cần __2 đường tín hiệu__ để truyền nhận dữ liệu, giúp đơn giản hóa việc kết nối trong hệ thống nhúng.


### Quan hệ giao tiếp:
__I2C cũng sử dụng mô hình Master – Slave:__

- Master là thiết bị khởi tạo và điều khiển truyền dữ liệu.
- Slave là thiết bị bị động, chỉ phản hồi khi được Master gọi đến.

I2C cho phép __nhiều Master__ và __nhiều Slave__ trên cùng một bus (Multi-Master – Multi-Slave).

Mỗi thiết bị trên bus I2C đều có __địa chỉ riêng__ (7 hoặc 10 bit) để Master định danh.


## Các đường tín hiệu:

__I2C sử dụng chỉ 2 dây:__

- __SCL (Serial Clock Line)__: Tín hiệu xung clock do Master tạo để đồng bộ dữ liệu.

- __SDA (Serial Data Line)__: Truyền dữ liệu nối tiếp giữa Master và Slave. Cả truyền và nhận đều diễn ra trên dây này.

Cả hai dây đều là __open-drain__ nên cần điện trở pull-up để hoạt động chính xác.

### Chế độ truyền và tốc độ truyền
- I2C chỉ hỗ trợ truyền dữ liệu __một chiều tại một thời điểm__ (half-duplex).

- Các thiết bị truyền và nhận lần lượt trên cùng đường dây SDA.

__Tốc độ truyền:__
- Standard Mode: 100 kbps
- Fast Mode: 400 kbps
- Fast Mode Plus: 1 Mbps

### Khoảng cách truyền

- Do dùng đường kéo (open-drain), I2C không thích hợp để truyền xa.

- Khoảng cách tối ưu: dưới 1 mét (với tốc độ cao).

- Nếu kéo dài dây tốc độ truyền phải giảm xuống và dễ xảy ra nhiễu, trễ tín hiệu và lỗi giao tiếp.

### Truyền dữ liệu

- Master kéo __SDA__ xuống __LOW__ trong khi __SCL__ vẫn __HIGH__ -> báo hiệu bắt đầu truyền. __(START)__

- Master gửi __địa chỉ Slave__ (7 hoặc 10 bit) + 1 __bit đọc/ghi__ (R/W). Sau đó, các thiết bị khác lắng nghe và chỉ thiết bị có địa chỉ trùng mới phản hồi. __(Addr + R/W)__

- Slave phản hồi bằng cách kéo __SDA__ xuống mức __LOW__ trong 1 chu kỳ clock để báo đã nhận địa chỉ. __(ACK)__

- Dữ liệu được truyền theo từng byte (8 bit) và mỗi byte đều có bit ACK sau đó để xác nhận từ Slave. __(DATA + ACK)__

- Để Dừng, SDA được kéo HIGH trong khi SCL đang HIGH -> báo hiệu kết thúc giao tiếp. __(STOP)__


## UART

### UART là gì?
__UART__ là một chuẩn giao tiếp nối tiếp không đồng bộ (asynchronous serial communication), được sử dụng phổ biến để truyền dữ liệu giữa vi điều khiển (MCU) và các thiết bị ngoại vi như module Bluetooth, GPS, máy tính, ...

Khác với SPI và I2C, UART __không cần tín hiệu clock__ chung giữa 2 thiết bị.


### Quan hệ giao tiếp:
- Giao tiếp diễn ra theo kiểu song phương giữa hai thiết bị ngang hàng, mỗi bên đều có thể truyền và nhận dữ liệu.

- Cần thiết lập trước các thông số giống nhau ở cả 2 bên (baudrate, số bit dữ liệu, parity, stop bit…).


## Các đường tín hiệu:

__UART sử dụng tối thiểu 2 đường tín hiệu chính:__

- TX (Transmit): Truyền dữ liệu từ thiết bị này sang thiết bị kia.

- RX (Receive): Nhận dữ liệu từ thiết bị kia về.

### Chế độ truyền và tốc độ truyền

- UART truyền và nhận dữ liệu đồng thời __(full-duplex)__, sử dụng hai đường riêng biệt: TX và RX. Giao tiếp hoàn toàn song song hai chiều.
 
- Tốc độ truyền được đo bằng đơn vị __baudrate__ (bps – bit per second).

- Các baudrate phổ biến: __9600__, 19200, 38400, 57600, __115200__ bps, …

### Khoảng cách truyền
Tùy thuộc vào điện áp, tốc độ truyền và loại cáp sử dụng:

- UART chuẩn (TTL 3.3V hoặc 5V): khoảng 1–5 mét.

- Nếu dùng RS-232 (chuẩn điện áp ±12V): lên đến 15 mét.

### Truyền dữ liệu

__UART__ truyền dữ liệu không có xung clock, nên cần quy ước định dạng trước. Một __frame__ dữ liệu UART bao gồm:

- __Start bit:__ Luôn là 1 bit LOW (0) báo hiệu bắt đầu truyền dữ liệu.

- __Data bits:__ Gồm 5, 6, 7, 8 hoặc 9 bit dữ liệu tùy cấu hình (thường là 8 bit).

- __Optional Parity bit:__ Bit kiểm tra chẵn lẻ, để phát hiện lỗi truyền (có hoặc không).

- __Stop bit(s):__ Là 1 hoặc 2 bit ở mức HIGH (1), báo hiệu kết thúc truyền 1 byte.