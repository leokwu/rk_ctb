//
// Created by leok on 2019/10/12.
//

#include "ctb_device.h"

static class CtbDeviceUnit ctb_device;

static rknn_context rknn_ctx;
static rknn_input_output_num in_out_num;
static rknn_tensor_attr tensor_attr[8];

static const char *MODEL_PATH = "../models/ssd/ssd_resnet_fpn_coco_pre_compile_quantity_dataset_coco.rknn";


int load_model(const char *model_path, uint32_t flag)
{
	int ret = 0;
	FILE *fp = NULL;
	int model_len = 0;
	unsigned char *model = NULL;

	/* Load model to bufffer */
	printf("loading model...\n");
	fp = fopen(model_path, "rb");
	if (fp == NULL) {
		printf("fopen %s fail!\n", model_path);
		ret = RKNN_ERR_MODEL_INVALID;
		goto exit;
	}

	fseek(fp, 0, SEEK_END);
	model_len = ftell(fp);
	model = (unsigned char *)malloc(model_len);
	if (!model) {
		printf("Malloc %d buffer fail!\n", model_len);
		ret = RKNN_ERR_MALLOC_FAIL;
		goto exit;
	}
	fseek(fp, 0, SEEK_SET);
	if (model_len != fread(model, 1, model_len, fp)) {
		printf("fread %s fail!\n", model_path);
		goto exit;
	}

	/* load model to npu */
	ret = rknn_init(&rknn_ctx, model, model_len, flag);
	if (ret < 0) {
		printf("rknn_init fail! ret=%d\n", ret);
		goto exit;
	}

	ret = rknn_query(rknn_ctx, RKNN_QUERY_IN_OUT_NUM, &in_out_num,
			sizeof(in_out_num));
	if (ret)
		goto exit;

	printf("input num = %u, output = num = %u\n",
			in_out_num.n_input, in_out_num.n_output);
	for (int i = 0; i < in_out_num.n_output; i++) {
		tensor_attr[i].index = i;
		ret = rknn_query(rknn_ctx, RKNN_QUERY_OUTPUT_ATTR,
				&tensor_attr[i], sizeof(rknn_tensor_attr));
		if (ret < 0) {
			goto exit;
		}
		printf("tensor index %d is %s, type = %02x, qnt_type = %02x\n",
				i, tensor_attr[i].name, tensor_attr[i].type, tensor_attr[i].qnt_type);
	}

exit:
	if (fp)
		fclose(fp);
	if (model)
		free(model);

	return ret;
}


int inference(void *in_data, int in_len)
{
	int ret = 0;
	rknn_input input;
	rknn_output outputs[in_out_num.n_output];

	memset(&input, 0x00, sizeof(input));
	input.index = 0;
	input.buf   = in_data;
	input.size  = in_len;
	input.pass_through = false;
	input.type  = RKNN_TENSOR_UINT8;
	input.fmt   = RKNN_TENSOR_NHWC;

	ret = rknn_inputs_set(rknn_ctx, 1, &input);
	if(ret < 0) {
		printf("rknn_input_set fail! ret=%d\n", ret);
		return ret;
	}

	ret = rknn_run(rknn_ctx, nullptr);
	if(ret < 0) {
		printf("rknn_run fail! ret=%d\n", ret);
		return ret;
	}

	memset(outputs, 0, sizeof(outputs));
	for (int i = 0; i < in_out_num.n_output; i++) {
		outputs[i].want_float = 1;
	}
	ret = rknn_outputs_get(rknn_ctx, in_out_num.n_output, outputs, NULL);
	if(ret < 0) {
		printf("rknn_outputs_get fail! ret=%d\n", ret);
		return ret;
	}

	for (int i = 0; i < in_out_num.n_output; i++) {
		LOGD("outputs[i].buf: %s outputs[i].size: %d", outputs[i].buf, outputs[i].size);
#if 0
		ret = tcp_server.send_data(outputs[i].buf, outputs[i].size);
		if (ret != outputs[i].size) {
			printf("send len error: %d; expect len: %d\n", ret, outputs[i].size);
			return -1;
		}
#endif
	}

	rknn_outputs_release(rknn_ctx, in_out_num.n_output, outputs);

	return 0;
}


int main(int argc, char *argv[])
{
	int i, ret, size;


	ret = load_model(MODEL_PATH, 0);
	if (ret) {
		LOGE("load model error\n");
		goto exit;
	}

	ctb_device.mBulkWriteBuffer[0] = 0x55;
	ctb_device.mBulkWriteBuffer[4] = 0x18;
	ctb_device.mBulkWriteBuffer[8] = 0x2;
	ctb_device.mBulkWriteBuffer[12] = 0xbb;
	ctb_device.mBulkWriteBuffer[20] = 0x22;

	while (1) {
		ret = ctb_device.readData(ctb_device.mBulkReadBuffer, sizeof(ctb_device.mBulkReadBuffer));		
		auto read_data = (CTB_PACKET_READ *) ctb_device.mBulkReadBuffer;
		DUMPDATA(ctb_device.mBulkReadBuffer, read_data->pkt_length);
		LOGD("read len: %d ctb_device.mBulkReadBuffer: %s", ret, ctb_device.mBulkReadBuffer);
		if (ret > 0) {
			ret = ctb_device.writeData(ctb_device.mBulkWriteBuffer, sizeof(ctb_device.mBulkWriteBuffer));
			LOGD("write len: %d", ret);
		}

	}
exit:
	if (rknn_ctx >= 0)
		rknn_destroy(rknn_ctx);
	return 0;
}
