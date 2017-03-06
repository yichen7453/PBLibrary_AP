#include "stdafx.h"

#include "libuvc.h"
#include "libuvc_internal.h"

uvc_error_t uvc_init(uvc_context_t **pctx, struct libusb_context *usb_ctx)
{
	//int ret = 0;
	uvc_error_t ret = UVC_SUCCESS;
	uvc_context_t *ctx = (uvc_context*)calloc(1, sizeof(*ctx));

	if (usb_ctx == NULL) {
		//ret = libusb_init(&ctx->usb_ctx);
		ctx->own_usb_ctx = 1;
		if (ret != 0) {
			ctx = NULL;
		}
	}
	else {
		ctx->own_usb_ctx = 0;
		ctx->usb_ctx = usb_ctx;
	}

	if (ctx != NULL) {
		*pctx = ctx;
	}

	return ret;
}