// HW01: HỆ THỐNG GIÁM SÁT LÒ NHIỆT CÔNG NGHIỆP

#include <stdio.h>
#include <stdint.h>

// TASK 1: POINTERS & MEMORY

void parse_config(const uint8_t *config_packet, int16_t *high_threshold) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
if( config_packet == NULL || high_threshold == NULL ) 
{
return 0
 };
int16_t temperature;
temperature = ((int16_t)config_packet[1] << 8) + config_packet[0];
*high_threshold = temperature;




    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

// TASK 2: COMPILER & VOLATILE

int16_t read_temperature_reg(void *hw_sensor_reg) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
if (hw_sensor_reg == NULL)
 { 
    return 0; 
}
volatile int16_t *ptr = (volatile int16_t *)hw_sensor_reg;
return *ptr;



    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

// TASK 3: DATA TYPES & BITWISE OPERATIONS

void control_output(uint8_t *control_reg, uint8_t fan_enable, uint8_t alarm_enable) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
if ( control_reg == Null ){

return ;

}

if ( fan_enable == 1 ) {

*control_reg |= ( 1 << 0 );

}

if ( fan_enable == 0) {

*control_reg &= ~ ( 1 << 0 );

} 
if ( alarm_enable == 1 ) {

*control_reg |= ( 1 << 1 );

}

if ( alarm_enable == 0) {

*control_reg &= ~ ( 1 << 1 );

} 





    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

// HÀM MAIN KIỂM TRA (Học viên giữ nguyên để chạy thử nghiệm, chỉ thay đổi input nếu cần)

int main() {

    // 1. Test Task 1
    int16_t threshold = 0;
    uint8_t mock_packet[2] = {0x64, 0x00};
    parse_config(mock_packet, &threshold); // Có thể thay mock_packet thành NULL để test chống crash khi gặp NULL
    printf("Threshold Parsed: %d C\n", threshold);

    // 2. Test Task 2
    volatile int16_t mock_hardware_sensor = 105; 
    int16_t current_temp = read_temperature_reg((void*)&mock_hardware_sensor);
    printf("Current Temp Read: %d C\n", current_temp);

    // 3. Test Task 3
    uint8_t system_control_register = 0xFC; // 1111 1100
    if (current_temp >= threshold) {
        // Yêu cầu: fan = 1, alarm = 0
        control_output(&system_control_register, 1, 0);
    }
    printf("Control Reg Output: 0x%02X\n", system_control_register);

    return 0;
}