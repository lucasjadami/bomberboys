# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'bomberboys_client/version'

Gem::Specification.new do |spec|
  spec.name          = "bomberboys_client"
  spec.version       = BomberboysClient::VERSION
  spec.authors       = ["Lucas Adami", "Rafael Regis"]
  spec.email         = ["lucasjadami@gmail.com", "rafael.regisp@gmail.com"]
  spec.description   = %q{Bomberboys client}
  spec.summary       = %q{Bomberboys client}
  spec.homepage      = ""
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rspec"
end
