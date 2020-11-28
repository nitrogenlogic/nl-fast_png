/*
 * Kinect data manipulation utilities for Ruby.
 * (C)2011 Mike Bourgeous
 *
 * References:
 * http://www.rubyinside.com/how-to-create-a-ruby-extension-in-c-in-under-5-minutes-100.html
 * http://www.ruby-doc.org/docs/ProgrammingRuby/html/ext_ruby.html
 */
#include <ruby.h>
#include <ruby/thread.h>
#include <libpng/png.h>
#include <zlib.h>

// Information passed into store_png_blocking()
struct kinpng_info {
	unsigned int w, h, d;
	unsigned char *data;
	png_structp png_ptr;
	png_infop info_ptr;
};

// The PngExt Ruby module
VALUE PngExt = Qnil;

// Adds a libpng error message to the error string.
static void error_func(png_structp png, const char *msg)
{
	VALUE *errstr = png_get_error_ptr(png);

	rb_warning("%s\n", msg);

	if(RSTRING_LEN(*errstr) > 0) {
		rb_str_cat2(*errstr, " - ");
	}
	rb_str_cat2(*errstr, "Err: ");
	rb_str_cat2(*errstr, msg);
}

// Adds a libpng warning message to the error string.
static void warn_func(png_structp png, const char *msg)
{
	VALUE *errstr = png_get_error_ptr(png);

	rb_warning("%s\n", msg);

	if(RSTRING_LEN(*errstr) > 0) {
		rb_str_cat2(*errstr, " - ");
	}
	rb_str_cat2(*errstr, "Warn: ");
	rb_str_cat2(*errstr, msg);
}

// Appends PNG data to the output buffer.
static void write_func(png_structp png, png_bytep data, png_size_t length)
{
	VALUE *outbuf = png_get_io_ptr(png);

	// FIXME: Wrap with rb_protect(), re-acquire the GVL
	rb_str_cat(*outbuf, data, length);
}

// Does nothing.
static void flush_func(png_structp png)
{
}

void *store_png_blocking(void *data)
{
	struct kinpng_info *info = data;
	png_bytep row_pointers[info->h];
	unsigned int span = info->w * (info->d / 8);
	int i;

	if(setjmp(png_jmpbuf(info->png_ptr))) {
		return NULL;
	}

	for(i = 0; i < info->h; i++, info->data += span) {
		row_pointers[i] = info->data;
	}
	png_set_swap(info->png_ptr);
	png_write_image(info->png_ptr, row_pointers);

	png_write_end(info->png_ptr, info->info_ptr);

	return data;
}

// Stores the given data as a grayscale image with depth 8 or 16, using
// compression settings optimized for speed (SUB filter, minimal zlib
// compression).  16-bit data is expected to be in little-endian order.
VALUE rb_store_png(VALUE self, VALUE width, VALUE height, VALUE depth, VALUE data)
{
	VALUE outbuf;
	VALUE errstr;
	VALUE ret;

	unsigned int w, h, d;
	png_structp png_ptr;
	png_infop info_ptr;

	Check_Type(data, T_STRING);
	w = NUM2UINT(width);
	h = NUM2UINT(height);
	d = NUM2UINT(depth);

	if(d != 8 && d != 16) {
		rb_raise(rb_eArgError, "Depth must be either 8 or 16 (got %u).", d);
	}

	if(RSTRING_LEN(data) < w * h * (d / 8)) {
		rb_raise(rb_eArgError, "Data must contain at least %u bytes (got %zd).", w * h * (d / 8), (ssize_t)RSTRING_LEN(data));
	}

	outbuf = rb_str_buf_new(0);

	if(!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &errstr, error_func, warn_func))) {
		rb_raise(rb_eStandardError, "Unable to create PNG writing structure: %s\n", RSTRING_PTR(errstr));
	}

	if(!(info_ptr = png_create_info_struct(png_ptr))) {
		png_destroy_write_struct(&png_ptr, NULL);
		rb_raise(rb_eStandardError, "Unable to create PNG info structure: %s\n", RSTRING_PTR(errstr));
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		rb_raise(rb_eStandardError, "A libpng error occurred: %s\n", RSTRING_PTR(errstr));
	}

	png_set_write_fn(png_ptr, &outbuf, write_func, flush_func);

	png_set_IHDR(png_ptr, info_ptr, w, h, d,
		     PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_filter(png_ptr, 0, PNG_FILTER_SUB);
	png_set_compression_level(png_ptr, Z_BEST_SPEED);

	png_write_info(png_ptr, info_ptr);

	// FIXME: this is accessing a Ruby string's contents without the GVL; is that a problem?
	ret = rb_thread_call_without_gvl(
			store_png_blocking,
			&(struct kinpng_info){
			.w = w, .h = h, .d = d, .data = RSTRING_PTR(data), .png_ptr = png_ptr, .info_ptr = info_ptr
			},
			NULL,
			NULL
			);
	if(ret == NULL) {
		rb_raise(rb_eStandardError, "A libpng error occurred while writing: %s\n", RSTRING_PTR(errstr));
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		rb_raise(rb_eStandardError, "A libpng error occurred: %s\n", RSTRING_PTR(errstr));
	}

	png_destroy_write_struct(&png_ptr, &info_ptr);

	return outbuf;
}

// Initializes the kinpng module.
void Init_png_ext()
{
	VALUE nl = rb_define_module("NL");
	VALUE fast_png = rb_define_module_under(nl, "FastPng");
	PngExt = rb_define_module_under(fast_png, "PngExt");

	rb_define_module_function(PngExt, "store_png", rb_store_png, 4);
}
