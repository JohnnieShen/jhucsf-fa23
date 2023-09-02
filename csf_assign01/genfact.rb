#! /usr/bin/env ruby

MODES = {
  :add => :+,
  :sub => :-,
  :mul => :*,
}

mode = ARGV.length > 0 ? ARGV[0].to_sym : MODES.keys[rand(3)]

range = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
if mode == :mul or (ENV.has_key?('SMALL') and ENV['SMALL'] == 'yes')
  range = 0xfffffffffffffffffffffffffffffff
end

left = rand(range)
right = rand(range)

if mode == :sub
  # make sure left operand is greater than right
  if left < right
    left, right = right, left
  end
end

raise "unknown mode: #{mode}" if !MODES.has_key?(mode)

op = MODES[mode]

result = left.send(op, right)
puts "#{left.to_s(16)} #{op} #{right.to_s(16)} = #{result.to_s(16)}"
