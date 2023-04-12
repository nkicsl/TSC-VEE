#include "execution.h"

void destroy(struct evmc_vm* vm)
{
    assert(vm != NULL);
    free(vm);
    vm=NULL;
}

evmc_capabilities_flagset get_capabilities(struct evmc_vm* vm)
{
    return EVMC_CAPABILITY_EVM1;
}

enum evmc_set_option_result set_option(struct evmc_vm* c_vm, char const* c_name, char const* c_value)
{
    if (strcmp(c_name, "O") == 0)
    {
        if (strcmp(c_value, "0") == 0)
        {
            c_vm->execute = execute;
            return EVMC_SET_OPTION_SUCCESS;
        }
        else if (strcmp(c_value, "2") == 0)
        {
            c_vm->execute = execute;
            return EVMC_SET_OPTION_SUCCESS;
        }
        return EVMC_SET_OPTION_INVALID_VALUE;
    }
    return EVMC_SET_OPTION_INVALID_NAME;
}

void vm_init(struct evmc_vm *vm)
{
    vm->abi_version = EVMC_ABI_VERSION;
    vm->name = "evmonec";
    vm->version = "0.0.0";
    vm->destroy = destroy;
    vm->execute = execute;
    vm->get_capabilities = get_capabilities;
    vm->set_option = set_option;
}