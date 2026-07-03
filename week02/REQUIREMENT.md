# HW02: KIẾN TRÚC PHẦN MỀM VÀ QUẢN LÝ BỘ NHỚ HỆ THỐNG XE ĐẠP ĐIỆN THÔNG MINH

## 1. MIÊU TẢ BÀI TOÁN

Trong hệ thống xe đạp điện thông minh (Smart Bike), bộ xử lý trung tâm cần quản lý rất nhiều tác vụ song song: từ việc giải mã dữ liệu trạng thái của xe, tối ưu hóa cấu trúc gọi hàm điều khiển động cơ, cho đến việc giám sát các điều kiện phần cứng khẩn cấp và quản lý tài nguyên bộ nhớ một cách nghiêm ngặt. 

Bài tập này yêu cầu học viên vận dụng các kỹ thuật nâng cao trong lập trình C nhúng để hoàn thiện một hệ thống điều khiển Smart Bike tối giản, chạy giả lập trên môi trường máy tính (GCC compiler).

---

## 2. YÊU CẦU

Học viên phải hoàn thành **5 nhiệm vụ** lập trình cùng **3 câu hỏi tự luận**.

### Task 1 (Struct, Union & Bit-fields): Định nghĩa cấu trúc Bit-fields thanh ghi 16-bit

Hoàn thiện cấu trúc nằm trong union `Bike_Status_t` để ánh xạ chính xác một thanh ghi trạng thái hệ thống có độ rộng 16-bit (`raw_value`) với quy định phân bổ bit như sau:
- **Bit [0]**: `PWR_ON` (Trạng thái nguồn xe - 1 bit).
- **Bit [2:1]**: `ASSIST_LEVEL` (Chế độ trợ lực điện với quy ước mã hóa: `0` = Eco, `1` = Normal, `2` = Sport - 2 bits).
- **Bit [6:3]**: `LIGHT_BRIGHT` (Độ sáng hệ thống đèn - 4 bits).
- **Bit [15:7]**: `RESERVED` (Vùng trống bảo mật - 9 bits).

---

### Task 2 (Con trỏ Hàm): Cấu trúc Table-driven bằng Mảng con trỏ hàm

Thay vì dùng cấu trúc rẽ nhánh `switch-case` hoặc `if-else` gây cồng kềnh và tốn thời gian thực thi của CPU:
- **2.1**: Khai báo và khởi tạo một mảng con trỏ hàm có tên `drive_modes` chứa địa chỉ của 3 hàm xử lý động cơ tương ứng: `drive_eco`, `drive_normal`, và `drive_sport`.
- **2.2**: Tại hàm `main`, thực hiện gọi hàm xử lý động cơ thích hợp trích xuất trực tiếp từ mảng `drive_modes` dựa vào giá trị của trường `ASSIST_LEVEL`. *(Lưu ý: Cần có cơ chế kiểm tra an toàn để tránh việc chỉ số mảng vượt quá giới hạn).*

---

### Task 3 (Callback): Thiết kế kiến trúc Callback khẩn cấp

Hoàn thiện hàm giám sát Pin `Battery_Monitor`:
- Khi điều kiện nhiệt độ của khối Pin vượt ngưỡng an toàn (> 40°C), hệ thống phải kích hoạt ngay hàm callback khẩn cấp được truyền vào thông qua con trỏ hàm `overheat_cb`.
- Yêu cầu kiểm tra tính hợp lệ của con trỏ hàm trước khi thực thi để tránh lỗi sụp đổ hệ thống.

---

### Task 4 (Bản đồ bộ nhớ & Phân tích Crash/Hard Fault): Khảo sát Bản đồ bộ nhớ & Debug Crash Lab

- **4.1 (Thực hành)**: Hoàn thiện hàm `crash_simulation` bằng cách viết một hàm đệ quy ngắn gọn nhằm chủ động ép hệ thống rơi vào trạng thái tràn ngăn xếp (**Stack Overflow**).
- **4.2 (Tự luận)**: Biên dịch và chạy chương trình. Quan sát log in địa chỉ bộ nhớ ở cuối màn hình và sắp xếp 3 biến: `BIKE_MODEL`, `total_odometer`, và `current_speed` vào đúng các phân vùng bộ nhớ vật lý tương ứng (`.rodata`, `.bss`, `Stack`).
- **4.3 (Tự luận)**: Khi bạn kích hoạt hàm `crash_simulation`, hiện tượng gì sẽ xảy ra với chương trình chạy trên máy tính? Nếu đoạn mã này chạy trên một vi điều khiển thực tế (ví dụ: ARM Cortex-M), CPU sẽ nhảy vào hàm ngắt ngoại lệ (Handler) nào để xử lý sự cố?

---

### Task 5 (Linker, Linker Script & Phân bổ Bộ nhớ Vật lý): Tối ưu hóa dung lượng với Linker Script (Tự luận)

Hệ thống Smart Bike cần lưu lịch sử hành trình đi lại của người dùng trong một mảng toàn cục lớn: `uint16_t trip_history[20000];` (chiếm khoảng 40 KB bộ nhớ). Bạn đứng trước 2 lựa chọn khai báo:
- **Cách 1**: `uint16_t trip_history[20000] = {0};`
- **Cách 2**: `uint16_t trip_history[20000] = {1, 2, 3};`

Giả sử vi điều khiển tích hợp trên xe có bộ nhớ lưu trữ chương trình (**Flash**) cực kỳ giới hạn, chỉ còn trống đúng **20 KB**, nhưng bộ nhớ chạy chương trình (**RAM**) vẫn còn trống tới **128 KB**.
- **Câu hỏi**: Cách khai báo nào (Cách 1 hay Cách 2) sẽ giúp chương trình biên dịch thành công và nạp được xuống chip mà không làm quá tải dung lượng bộ nhớ Flash? Giải thích ngắn gọn dựa trên bản chất của phân vùng `.data` và `.bss`.

---

## 3. RÀNG BUỘC KỸ THUẬT

- **Cú pháp**: Bắt buộc sử dụng các toán tử giải con trỏ và toán tử bitwise khi thao tác trên thanh ghi (nếu có).
- **Quản lý mã nguồn**: Nghiêm cấm sử dụng các biến toàn cục không cần thiết để chia sẻ dữ liệu giữa các hàm nhằm đảm bảo tính đóng gói của kiến trúc phần mềm nhúng.
- **Sản phẩm nộp bài**: 
  1. File mã nguồn `main.c` đã hoàn thiện toàn bộ các vùng mã nguồn yêu cầu, biên dịch thành công không có Warning/Error.
  2. File text `answer.txt` chứa câu trả lời cho các câu hỏi tự luận ở **Task 4 (4.2, 4.3)** và **Task 5**.

---

## 4. TESTCASE

Học viên sử dụng dữ liệu kiểm thử dưới đây để đối chiếu với kết quả hiển thị trên Terminal của mình.

## Testcase 1 (Kiểm tra Giải mã Thanh ghi)

### Input

```text
my_bike.raw_value = 0x0025;
```

Kết quả:

```text
REGISTER STRUCTURE: 
PWR_ON: 1 | ASSIST_LEVEL: 2 | LIGHT_BRIGHT: 4
```

---

## Testcase 2 (Kiểm tra Mảng con trỏ hàm)

### Input

```text
Trường ASSIST_LEVEL sau khi giải mã nhận giá trị bằng 2.
```

Kết quả:

```text
ENGINE CONTROLLING: 
Bike Mode: SPORT.
```

---

## Testcase 3 (Kiểm tra Kiến trúc Callback)

### Input

```text
Hàm Battery_Monitor chạy với giả lập nhiệt độ pin hiện tại là 45°C (vượt ngưỡng an toàn 40°C).
```

Kết quả:

```text
[WARNING] Battery is overheated!
```

---

## Testcase 4 (Kiểm tra Địa chỉ Bộ nhớ)

### Input

```text
Chương trình chạy bình thường ở điều kiện không crash.
```

Kết quả:

```text
MEMORY ADDRESSES OF VARIABLES: 
BIKE_MODEL (ROM/.rodata):   0x...
total_odometer (RAM/.bss):  0x...
current_speed (RAM/Stack):  0x...
```

---

## 5. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY THỬ

Học viên mở Terminal tại thư mục chứa file main.c và sử dụng Trình biên dịch gcc để chạy chương trình:

```text
# 1. Biên dịch mã nguồn thành file thực thi 'main'
gcc main.c -o main

# 2. Chạy chương trình
./main
```

---

# Chúc các bạn làm bài vui vẻ!