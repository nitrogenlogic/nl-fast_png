require "bundler/gem_tasks"
require 'rake/extensiontask'

task :default => :spec

Rake::ExtensionTask.new 'png_ext' do |ext|
  ext.name = 'png_ext'
  ext.ext_dir = 'ext/nl-fast_png'
  ext.lib_dir = 'lib/nl/fast_png'
end
