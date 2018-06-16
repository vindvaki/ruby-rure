# ruby-rure

A simple native ruby extension wrapper around [the rust regex crate's C-api](https://github.com/rust-lang/regex/tree/master/regex-capi).

There are cases where this gem will be faster than Ruby's built in `Regexp`, and cases where it will be slower. Refer to [rust-lang/regex/PERFORMANCE.md](https://github.com/rust-lang/regex/blob/master/PERFORMANCE.md) for advice.

As a rule of thumb, you probably don't need this gem. It is probably safe to use.

## Installation

**TODO:** Write about installing `librure`

**TODO:** Publish gem

Add this line to your application's Gemfile:

```ruby
gem 'rure'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install ruby-rure

## Usage

Refer to [the regex crate's docs](https://docs.rs/regex/1.0.0/regex/) for the regular expression syntax (it differs from what ruby supports).

```ruby
require 'rure'

regex = Rure::Regex.new('pat(.)rn')
haystack = 'string that contains pattern'

# existence test
regex.match?(haystack) # => True

# simple match
regex.find(haystack).to_s # => 'pattern'

# capture groups
regex.find_captures(haystack)[1].to_s # => 't'
```

## Development

### Install dependencies

```bash
bundle install --binstubs --path vendor/bundle
```

### Recompile native extension
```bash
bin/rake compile
```

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/vindvaki/ruby-rure.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
