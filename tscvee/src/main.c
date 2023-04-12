#include <sys/time.h>

#include "cJSON.h"

#include "vm.h"
#include "instructions_traits.h"

int run(struct evmc_vm *vm, enum evmc_revision rev, int64_t gas, char *code_hex, char *input_hex, bool create, bool bench)
{
    printf("%s with %ld gas limit\n", create ? "Creating and executing on " : "Executing on ", gas);

    size_t code_size = strlen(code_hex) / 2 - 1;
    byte *code = (byte *)malloc(sizeof(byte) * code_size);
    from_hex(code_hex, code);

    size_t input_size = strlen(input_hex) / 2 - 1;
    byte *input = (byte *)malloc(sizeof(byte) * input_size);
    from_hex(input_hex, input);

    // A mock interface for interacting with the blockchain
    MockedHost* host = (MockedHost*)malloc(sizeof(MockedHost));
    MockedHost_init(host);

    struct evmc_message* msg = (struct evmc_message*)malloc(sizeof(struct evmc_message));
    EVMCMessage_init_v(msg, gas, input, input_size);

    // if(create){

    // }
    // printf("\n");

    struct evmc_result result = vm->execute(vm, host, rev, msg, code, code_size);

    if(bench){
        struct timeval start, end;
        gettimeofday(&start, NULL);

        for(int i = 0; i < 1000; ++i)
            vm->execute(vm, host, rev, msg, code, code_size);
            
        gettimeofday(&end, NULL);
        long int bench_time = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
        printf("Time: %ld us (avg of 1000 iterations)\n", bench_time);
    }

    int64_t gas_used = msg->gas - result.gas_left;
    printf("\nResult: %d\nGas used: %ld\n", result.status_code, gas_used);

    if (result.status_code == EVMC_SUCCESS || result.status_code == EVMC_REVERT)
    {
        size_t outlen = result.output_size * 2 + 2;
        char *output = (char *)malloc(sizeof(char) * outlen);
        hex(result.output_data, result.output_size, output);
        printf("Output: ");
        for (int i = 0; i < outlen; i++)
        {
            printf("%c", output[i]);
        }
        printf("\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // Read file and get input data
    FILE *fp = NULL;
    fp = fopen("../args/transferFrom.json", "r");
    fseek(fp, 0L, SEEK_END);
    long flen = ftell(fp);
    char *p = (char *)malloc(flen + 1);
    if (p == NULL)
    {
        fclose(fp);
        printf("Input data is Empty!");
        return -1;
    }
    fseek(fp, 0L, SEEK_SET);
    fread(p, flen, 1, fp);
    p[flen] = 0;
    // printf("Bytecode: %s\n", p);
    // printf("Length: %ld\n", flen);

    // Parse json data
    cJSON *cjson_content = NULL;
    cJSON *cjson_bytecode = NULL;
    cJSON *cjson_input = NULL;
    cJSON *cjson_gas = NULL;
    cjson_content = cJSON_Parse(p);
    if (cjson_content == NULL)
    {
        printf("Parse fail.\n");
        return -1;
    }
    cjson_bytecode = cJSON_GetObjectItem(cjson_content, "bytecode");
    cjson_input = cJSON_GetObjectItem(cjson_content, "input");
    cjson_gas = cJSON_GetObjectItem(cjson_content, "gas");
    // printf("Bytecode: %s\n", cjson_bytecode->valuestring);
    // printf("Input: %s\n", cjson_input->valuestring);
    // printf("Gas: %d\n", cjson_gas->valueint);

    // create VM for execution
    struct evmc_vm *vm = (struct evmc_vm *)malloc(sizeof(struct evmc_vm));
    vm_init(vm);

    enum evmc_revision rev = EVMC_LONDON;

    bool create = false;
    bool bench = true;

    // Initialize instruction table
    GasCostTable_init(gas_costs);
    TraitsTable_init(traits);

    run(vm, rev, cjson_gas->valueint, cjson_bytecode->valuestring, cjson_input->valuestring, create, bench);

    // cJSON_Delete(cjson_content);
    // cJSON_Delete(cjson_bytecode);
    // cJSON_Delete(cjson_input);
    // cJSON_Delete(cjson_gas);
}
