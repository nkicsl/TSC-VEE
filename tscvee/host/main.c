#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <tscvee_ta.h>

/* TEE resources */
struct tee_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

void prepare_tee_session(struct tee_ctx *ctx){
	TEEC_UUID uuid = TA_TSCVEE_UUID;
	uint32_t origin;
	TEEC_Result res;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, origin);
}

void terminate_tee_session(struct tee_ctx *ctx){
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

TEEC_Result tscvee_run(struct tee_ctx *ctx){
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	
	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	printf("Invoking TSCVEE TA to execute smart contract\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_TSCVEE_CMD_RUN, &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TSCVEE TA invoke failed with code 0x%x origin 0x%x", res, origin);
	printf("Smart contract has been executed successly\n");
}

TEEC_Result tscvee_bench(struct tee_ctx *ctx){
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	
	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	printf("Invoking TSCVEE TA to execute smart contract and bench the performance\n");
	res = TEEC_InvokeCommand(&ctx->sess, TA_TSCVEE_CMD_BENCH, &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TSCVEE TA invoke failed with code 0x%x origin 0x%x", res, origin);
	printf("Smart contract has been executed and benched successly\n");
}

int main(int argc, char *argv[])
{
	struct tee_ctx ctx;
	TEEC_Result res;

	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);
	
	// optee_tscvee run/bench
	if(0 == strcmp(argv[1], "run")){
		tscvee_run(&ctx);
	}
	else if(0 == strcmp(argv[1], "bench")){
		tscvee_bench(&ctx);
	}
	else{
		printf("Error: Unkown argument!\n");
	}
	
	printf("Terminate session with the TA\n");
	terminate_tee_session(&ctx);
	return 0;
}
