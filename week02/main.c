// HW02: KIẾN TRÚC PHẦN MỀM VÀ QUẢN LÝ BỘ NHỚ SMART BIKE

#include <stdio.h>
#include <stdint.h>

// TASK 1: STRUCT, UNION & BIT-FIELDS

typedef union {
    uint16_t raw_value;
    struct {
        // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
        uint16_t PWR_ON       : 1;  
        uint16_t ASSIST_LEVEL : 2;  
        uint16_t LIGHT_BRIGHT : 4;  
        uint16_t RESERVED     : 9;



        // HỌC VIÊN KẾT THÚC VIẾT CODE
    } fields;
} Bike_Status_t;


// TASK 2: FUNCTION POINTERS & TABLE-DRIVEN ARCHITECTURE

void drive_eco(void) {
    printf("Bike Mode: ECO.\n");
}
void drive_normal(void) {
    printf("Bike Mode: NORMAL.\n");
}
void drive_sport(void) {
    printf("Bike Mode: SPORT.\n");
}

// HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
typedef void (*drive_func_t)(void);

static const drive_func_t drive_modes[] = {
    drive_eco,
    drive_normal,
    drive_sport
};

void execute_drive_mode(Bike_Status_t status) {
    uint8_t current_mode = status.fields.ASSIST_LEVEL;

    if (current_mode < 3) {
        drive_modes[current_mode](); 
    } 
    else {
        printf("[ERROR] Chế độ trợ lực không hợp lệ: %d (Vượt quá giới hạn mảng)!\n", current_mode);
    }
}



// HỌC VIÊN KẾT THÚC VIẾT CODE


// TASK 3: ARCHITECTURE CALLBACKS

void Battery_Monitor(void (*overheat_cb)(void)) {
    int battery_temp = 45; 
    
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
typedef void(*overheat_callback_t)(uint_8_t temp);
    void Battery_Monitor(uint_8_ current_temperature, overheat_callback_t overheat_cb) {
        if (current_temperature > 40) {
        if (overheat_cb != NULL) {
            overheat_cb(current_temperature);
        }
    }
}



    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

void Critical_Battery_Handler(void) {
    printf("[WARNING] Battery is overheated!\n");
}


// TASK 4: MEMORY MAP & CRASH LAB

const char BIKE_MODEL[] = "E-Bike X2026"; 
uint32_t total_odometer = 0;             

void crash_simulation(void) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    uint8_t buffer[1024]; // để chiếm 1KB
    buffer[0] = 5;
    crash_simulation();
}




    // HỌC VIÊN KẾT THÚC VIẾT CODE
}


// HÀM MAIN KIỂM TRA (Học viên giữ nguyên để chạy thử nghiệm)

int main() {
    Bike_Status_t my_bike;
    
    // 1. Test Task 1
    my_bike.raw_value = 0x0025; 
    printf("REGISTER STRUCTURE: \n");
    printf("PWR_ON: %d | ASSIST_LEVEL: %d | LIGHT_BRIGHT: %d\n\n", 
           my_bike.fields.PWR_ON, my_bike.fields.ASSIST_LEVEL, my_bike.fields.LIGHT_BRIGHT);

    // 2. Test Task 2
    printf("ENGINE CONTROLLING: \n");
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY




    // HỌC VIÊN KẾT THÚC VIẾT CODE

    // 3. Test Task 3
    Battery_Monitor(Critical_Battery_Handler);
    printf("\n");

    // 4. Test Task 4
    int current_speed = 25; 
    printf("MEMORY ADDRESSES OF VARIABLES \n");
    printf("BIKE_MODEL (ROM/.rodata):   %p\n", (void*)BIKE_MODEL);
    printf("total_odometer (RAM/.bss):  %p\n", (void*)&total_odometer);
    printf("current_speed (RAM/Stack):  %p\n", (void*)&current_speed);

    // Bỏ comment dòng dưới để chạy thử bài Crash Lab
    // crash_simulation();

    return 0;
}
