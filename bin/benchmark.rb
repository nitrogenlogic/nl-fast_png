#!/usr/bin/env ruby

require 'rubygems'
require 'bundler/setup'

require 'benchmark'

require 'nl/fast_png'

data = 256.times.map(&:chr).join * 1024
elapsed = Benchmark.realtime do
  10000.times do
    NL::FastPng.store_png(512, 512, 8, data)
  end
end

puts "Elapsed: #{elapsed} seconds"
puts "Speed: #{10000 / elapsed} per second"
