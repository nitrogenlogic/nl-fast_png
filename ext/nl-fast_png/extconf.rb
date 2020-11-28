require 'mkmf'

raise 'libpng not found; please install libpng-dev' unless have_library("png", "png_create_write_struct")

with_cflags("#{$CFLAGS} -O3 -Wall -Wextra #{ENV['EXTRACFLAGS']} -std=c99") do
  create_makefile('nl/fast_png/png_ext')
end
