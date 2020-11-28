# NL::FastPNG

NL::FastPng is a C-based extension that uses libpng in a simplified
configuration for faster performance on black and white images, at the expense
of slightly larger file sizes.

This code was used by the Nitrogen Logic Depth Camera Controller to generate
PNG images from realtime depth data on limited hardware.

## License

NL::FastPng is licensed under a 2-clause BSD license.  See the LICENSE file for
details.  NL::FastPng is &copy; 2011-2020, Mike Bourgeous.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'nl-fast_png'
```

And then execute:

    $ bundle install

Or install it yourself as:

    $ gem install nl-fast_png

## Usage

```
require 'nl/fast_png'

# Store a 2x2 checkerboard pattern to /tmp/x.png
File.write('/tmp/x.png', NL::FastPng.store_png(2, 2, 8, "\xff\x00\x00\xff"))

# Store a 256x256 horizontal gradient to /tmp/grad.png
File.write('/tmp/grad.png', NL::FastPng.store_png(256, 256, 8, 256.times.map(&:chr).join * 256))
```
