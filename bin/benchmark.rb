#!/usr/bin/env ruby

require 'rubygems'
require 'bundler/setup'

require 'benchmark'

require 'nl/fast_png'

THREADS=ENV['THREADS']&.to_i || 8
COUNT=ENV['COUNT']&.to_i || 1000

elapsed = Benchmark.realtime do
  threads = THREADS.times.map {
    Thread.new do
      data = 256.times.map(&:chr).join * 1024
      COUNT.times do
        NL::FastPng.store_png(512, 512, 8, data)
      end
    end
  }

  threads.each(&:join)
end

puts "Elapsed: #{elapsed} seconds"
puts "Speed: #{(THREADS * COUNT) / elapsed} per second"
