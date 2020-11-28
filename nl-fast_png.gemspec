require_relative 'lib/nl/fast_png/version'

Gem::Specification.new do |spec|
  spec.name          = "nl-fast_png"
  spec.version       = NL::FastPng::VERSION
  spec.authors       = ["Mike Bourgeous"]
  spec.email         = ["mike@mikebourgeous.com"]

  spec.summary       = %q{A fast libpng-based compressor for black and white images.}
  spec.homepage      = 'https://github.com/nitrogenlogic/nl-fast_png'
  spec.required_ruby_version = Gem::Requirement.new(">= 2.3.0")

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata["source_code_uri"] = spec.homepage

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path('..', __FILE__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]

  spec.extensions = [
    'ext/nl-fast_png/extconf.rb'
  ]

  spec.add_development_dependency 'rake-compiler'
  spec.add_development_dependency 'pry'
end
