
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "toycar_int8_data/toycar_int8_input_data.h"
#include "toycar_int8_data/toycar_int8_model_data.h"
#include "toycar_int8_data/toycar_int8_model_settings.h"
#include "toycar_int8_data/toycar_int8_output_data_ref.h"

extern "C" {
#include "runtime.h"
#include "uart.h"
#include "uart.c"
#include "terminate_benchmark.h"
}

constexpr size_t tensor_arena_size = 256 * 1024;
alignas(16) uint8_t tensor_arena[tensor_arena_size];

int run_test()
{
    //tflite::MicroErrorReporter micro_error_reporter;
    //tflite::ErrorReporter *error_reporter = &micro_error_reporter;

    const tflite::Model *model = tflite::GetModel(toycar_int8_model_data);

    static tflite::MicroMutableOpResolver<1> resolver;
    resolver.AddFullyConnected();

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size);

    uart_printf("Started Program\n");

    if (interpreter.AllocateTensors() != kTfLiteOk)
    {
        //TF_LITE_REPORT_ERROR(error_reporter, "ERROR: In AllocateTensors().");
        return -1;
    }

    for (size_t i = 0; i < toycar_int8_data_sample_cnt; i++)
    {
        memcpy(interpreter.input(0)->data.int8, (int8_t *)toycar_int8_input_data[i], toycar_int8_input_data_len[i]);
        
        //start_cycle_count(); //RD cycle is illegal, causes a lock on attempted offload

        if (interpreter.Invoke() != kTfLiteOk)
        {
            //TF_LITE_REPORT_ERROR(error_reporter, "ERROR: In Invoke().");
            return -1;
        }

        int32_t sum = 0;
        for (size_t j = 0; j < toycar_int8_input_data_len[i]; j++)
        {
            int32_t diff1 = (int8_t)toycar_int8_input_data[i][j] - (int8_t)interpreter.output(0)->data.int8[j];
            int32_t square = diff1*diff1;
            sum += square;
        }
        sum /= toycar_int8_input_data_len[i];

        int32_t diff = abs(sum - toycar_int8_output_data_ref[i]);

        
        //store_result_int(diff);
        
        if (diff > 1)
        {
            #if defined(PRINT_OUTPUTS)
            uart_printf("ERROR: at #%d, sum %d ref %d diff %d \n", i, sum, toycar_int8_output_data_ref[i], diff);
            #endif
            return -1;
        }
        else
        {
            #if defined(PRINT_OUTPUTS)
            uart_printf("Sample #%d pass, sum %d ref %d diff %d \n", i, sum, toycar_int8_output_data_ref[i], diff);
            #endif
        }
    }

    return 0;
}

extern "C" {
int mlonmcu_run();
#include <stdlib.h>
#include <stdio.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NUM_BLOCKS 10

int test_malloc() {
    printf("Malloc Test Start\n");

    // 1. Basic allocation
    int *a = (int *)malloc(sizeof(int));
    if (!a) {
        printf("FAIL: malloc returned NULL\n");
        return 1;
    }
    *a = 42;
    printf("PASS: Basic allocation, value = %d\n", *a);

    // 2. Multiple allocations
    int *arr[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        arr[i] = (int *)malloc(sizeof(int) * (i + 1));
        if (!arr[i]) {
            printf("FAIL: malloc failed at block %d\n", i);
            return 1;
        }
        memset(arr[i], i, sizeof(int) * (i + 1));
    }
    printf("PASS: Multiple allocations\n");

    // 3. Check data integrity
    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < (i + 1) * sizeof(int); j++) {
            if (((uint8_t *)arr[i])[j] != (uint8_t)i) {
                printf("FAIL: Data corruption in block %d\n", i);
                return 1;
            }
        }
    }
    printf("PASS: Data integrity\n");

    // 4. Free and reuse
    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(arr[i]);
    }
    printf("PASS: Free completed\n");

    // 5. Re-allocation after free
    int *b = (int *)malloc(sizeof(int));
    if (!b) {
        printf("FAIL: malloc failed after free\n");
        return 1;
    }
    *b = 99;
    printf("PASS: Re-allocation works, value = %d\n", *b);

    // 6. Alignment test
    if (((uintptr_t)b % sizeof(void *)) != 0) {
        printf("FAIL: Pointer not properly aligned\n");
        return 1;
    }
    printf("PASS: Alignment OK\n");

    free(a);
    free(b);

    // 7. Stress test (allocate until failure)
    int count = 0;
    while (malloc(32) != NULL) {
        count++;
    }
    printf("INFO: Allocated %d blocks before exhaustion\n", count);

    printf("Malloc Test End\n");
    return 0;
}

#include "cfu.h"

#define CFU_OPCODE_PUSH_WEIGHTS        0b0010000
#define CFU_OPCODE_SET_CODEBOOK_2B     0b0100000
#define CFU_OPCODE_SET_CODEBOOK_4B     0b0101000
#define CFU_OPCODE_SET_CODEBOOK_16B    0b0111000
#define CFU_OPCODE_ALU_MAC             0b1000000
#define CFU_OPCODE_ALU_RST             0b1001000
#define CFU_OPCODE_MAC_READ            0b1010000
#define CFU_OPCODE_MAC_READ_NO_RESET   0b1010100
#define CFU_OPCODE_DEBUG_DUMP          0b1010010


#include <riscv_vector.h>
#include <stddef.h>

void debug_vint8(vint8m1_t v, size_t vl);
void debug_vint8(vint8m1_t v, size_t vl) {
    int8_t buf[vl];
    __riscv_vse8_v_i8m1(buf, v, vl);

    for (size_t i = 0; i < vl; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

void debug_vuint8(vuint8m1_t v, size_t vl);
void debug_vuint8(vuint8m1_t v, size_t vl) {
    uint8_t buf[vl];
    __riscv_vse8_v_u8m1(buf, v, vl);

    for (size_t i = 0; i < vl; i++) {
        // printf("%u ", buf[i]);
        printf("0x%02x ", buf[i]);
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    uart_printf("Hello!\n");
    cfu_op0_hw(CFU_OPCODE_ALU_RST, 0, 0);
    cfu_op0_hw(CFU_OPCODE_SET_CODEBOOK_2B, 255, 255);
    cfu_op0_hw(CFU_OPCODE_PUSH_WEIGHTS, 0, 0);
    // cfu_op0_hw(CFU_OPCODE_PUSH_WEIGHTS, 255, 255);
    int32_t acc;
    cfu_op0_hw(CFU_OPCODE_ALU_MAC, 123, 456);
    acc = cfu_op0_hw(CFU_OPCODE_MAC_READ, 0, 0);
    printf("acc=%d\n", acc);
    cfu_op0_hw(CFU_OPCODE_ALU_MAC, 123, 456);
    acc = cfu_op0_hw(CFU_OPCODE_MAC_READ, 0, 0);
    printf("acc=%d\n", acc);
    cfu_op0_hw(CFU_OPCODE_ALU_MAC, 123, 456);
    acc = cfu_op0_hw(CFU_OPCODE_MAC_READ_NO_RESET, 0, 0);
    printf("acc=%d\n", acc);
    cfu_op0_hw(CFU_OPCODE_ALU_MAC, 123, 456);
    acc = cfu_op0_hw(CFU_OPCODE_MAC_READ_NO_RESET, 0, 0);
    printf("acc=%d\n", acc);
    // ENDIANESS!
    // const uint32_t packed_w[2] = {0x013456, 0x789abcdf};
    const uint32_t packed_w[2] = {0x76543210, 0xfedcba98};
    uint8_t* packed_w_ptr = (uint8_t*)(&packed_w[0]);
    size_t vl_bytes = __riscv_vsetvl_e8m1(8); // number of packed bytes
    printf("vl_bytes=%u\n", vl_bytes);
    vuint8m1_t vpacked = __riscv_vle8_v_u8m1(packed_w_ptr, vl_bytes);
    printf("vpacked:\n");
    debug_vuint8(vpacked, vl_bytes);
    vuint8m1_t vidx = __riscv_vundefined_u8m1();
    size_t vl_bytes_unpacked = vl_bytes * 4;
    printf("vl_bytes_unpacked=%u\n", vl_bytes_unpacked);
    size_t vl = 0;
    // asm("vsetvli %1, %3, e8, m1, ta, ma\n"  ".insn  r CUSTOM_1, 0x0, 0x0, %0, v0, %2\n" : "+vr"(vidx), "=r"(vl) : "vr" (vpacked), "r" (vl_bytes_unpacked));
    asm("vsetvli %0, %1, e8, m1, ta, ma" : "=r"(vl) : "r" (vl_bytes_unpacked));
    printf("vl=%u\n", vl);
    printf("vidx:\n");
    debug_vuint8(vidx, vl_bytes_unpacked);
    // asm(".insn r CUSTOM_1, 0x0, 0x0, %0, v0, %1" : "+vr"(vidx) : "vr" (vpacked));
    asm(".insn r CUSTOM_1, 0x0, 0x1, %0, v0, %1" : "+vr"(vidx) : "vr" (vpacked));
    printf("vidx:\n");
    debug_vuint8(vidx, vl_bytes_unpacked);
    // int ret = mlonmcu_run();
    // test_malloc();
    // uart_printf("Bye!\n");
    // printf("Bye!\n");
    // exit(42);
    int ret = 0;
    // int ret = run_test();
    if (ret != 0)
    {
        #if defined(PRINT_OUTPUTS)
        uart_printf("Test Failed!\n");
        #endif
        benchmark_failure();

    }
    else
    {
        #if defined(PRINT_OUTPUTS)
        uart_printf("Test Success!\n");
        #endif
        benchmark_success(); 
    }

    return ret;
}
