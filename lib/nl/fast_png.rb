require "nl/fast_png/version"

module NL
  module FastPng
    # Returns a String containing a compressed PNG version of the given
    # grayscale +data+, which has +width+ columns, +height+ lines, and has a
    # bit depth of +depth+ (either 8 or 16).
    #
    # Example:
    #     File.write('/tmp/x.png', NL::FastPng.store_png(2, 2, 8, "\xff\x00\x00\xff"))
    def self.store_png(width, height, depth, data)
      PngExt.store_png(width, height, depth, data)
    end
  end
end

require_relative 'fast_png/png_ext'
