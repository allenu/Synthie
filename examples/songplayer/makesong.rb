#!/usr/bin/env ruby

if ARGV.length != 2
    puts "Usage:\n\n  ./makesong.rb <input_song.txt> <output_file.song>\n"
    exit -1
end

input_filename = ARGV[0]
output_filename = ARGV[1]

file = File.open(input_filename)

waveforms = ["silence", "sine", "triangle", "square", "sawtooth", "noise"]
notes = ["c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"]

section = "none"
pattern_index = 0
patterns = []
pattern = nil
instruments = []
bpm = 120.0
ticks_per_beat = 1
ticks_per_pattern = 64

last_note_for_channel = [0,0,0,0,0,0]
last_octave_for_channel = [0,0,0,0,0,0]
last_instrument_for_channel = [0,0,0,0,0,0]

file.each_line { |line|
    line.chomp!

    # remove comments
    line.sub!(/\#.*/,"")

    if line =~ /\s*^\.(\S+)(.*)/
        section = $1
        rest = $2

        if section == "pattern"
            if rest =~ /(\d+)/
                pattern_index = $1.to_i
                printf "pattern index: %d\n", pattern_index

                if pattern != nil
                    patterns << pattern
                end
                pattern = []
            end
        end
    else

        case section
        when "track_info"
            if line =~ /(\S+):(.*)/
                label = $1
                rest = $2

                case label
                when "bpm"
                    bpm = $2.to_f
                when "ticks_per_beat"
                    ticks_per_beat = $2.to_i
                when "ticks_per_pattern"
                    ticks_per_pattern = $2.to_i
                end
            end

        when "instruments"
            if line =~ /(\S+):\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/
                name = $1
                waveform_name = $2
                attack = $3.to_f
                attack_gain = $4.to_f
                decay = $5.to_f
                sustain_gain = $6.to_f
                release = $7.to_f

                waveform_index = waveforms.index(waveform_name)

                printf("instrument: %s   %s  %.2f %.2f %.2f %.2f %.2f\n", name, waveform_index, attack, attack_gain, decay, sustain_gain, release)

                instrument = {
                    :name => name, 
                    :waveform_index => waveform_index,
                    :attack => attack,
                    :attack_gain => attack_gain,
                    :decay => decay,
                    :sustain_gain => sustain_gain,
                    :release => release
                }
                instruments << instrument
            end
        when "pattern"
            if line =~ /(\d+):\s+(\d+)\s+(\S+)\s+(\S+)/
                pattern_tick = $1.to_i
                channel_index = $2.to_i
                instrument_name = $3
                note_name = $4

                note = 0
                octave = 5
                if note_name =~ /(\S\#?)(\d+)/
                    note = $1.downcase
                    octave = $2.to_i
                    note = notes.index(note)
                end

                # printf("pattern command: line %d, channel %d, instrument %s, note %s\n", pattern_tick, channel_index, instrument_name, note_name)

                instrument = instruments.find { |i| i[:name] == instrument_name }
                if instrument == nil
                    puts "ERROR: instrument not found: " + instrument_name
                    exit -1
                else

                    instrument_index = instruments.index(instrument)
                    command = { 
                        :tick_index => pattern_tick, 
                        :channel => channel_index, 
                        :action_type => 0,
                        :instrument_index => instrument_index,
                        :note => note,
                        :octave => octave }

                    last_note_for_channel[channel_index] = note
                    last_octave_for_channel[channel_index] = octave
                    last_instrument_for_channel[channel_index] = instrument_index

                    pattern << command
                end
            elsif line =~ /(\d+):\s+(\d+)\s+\./
                # Repeat note
                pattern_tick = $1.to_i
                channel_index = $2.to_i
                command = { 
                    :tick_index => pattern_tick, 
                    :channel => channel_index, 
                    :action_type => 0,
                    :note => last_note_for_channel[channel_index],
                    :octave => last_octave_for_channel[channel_index],
                    :instrument_index => last_instrument_for_channel[channel_index]
                }
                pattern << command
            elsif line =~ /(\d+):\s+(\d+)\s+----/
                pattern_tick = $1.to_i
                channel_index = $2.to_i
                command = { 
                    :tick_index => pattern_tick, 
                    :channel => channel_index, 
                    :action_type => 1,
                    # these don't matter
                    :instrument_index => 0,
                    :note => 0,
                    :octave => 0
                }
                pattern << command
            elsif line =~ /(\d+):\s+(\d+)\s+(\S+)/
                # channel name and note
                pattern_tick = $1.to_i
                channel_index = $2.to_i
                note_name = $3

                note = 0
                octave = 5
                if note_name =~ /(\S\#?)(\d+)/
                    note = $1.downcase
                    octave = $2.to_i
                    note = notes.index(note)
                end

                command = { 
                    :tick_index => pattern_tick, 
                    :channel => channel_index, 
                    :action_type => 0,
                    :instrument_index => last_instrument_for_channel[channel_index],
                    :note => note,
                    :octave => octave
                }
                pattern << command
            end
        when "pattern_order"
            # TODO: 
        end
    end
}

if pattern != nil
    patterns << pattern
end

# Sort each pattern and spit them out

output_file = File.open(output_filename, "wb")

blob = ""

sig = "SNG\0"
output_file.write(sig)

version = [0, 0].pack('S2')
output_file.write(version)

bpm_data = [bpm].pack('D')
output_file.write(bpm_data)

ticks_per_beat_data = [ticks_per_beat].pack('S')
output_file.write(ticks_per_beat_data)

num_instruments_data = [ instruments.length ].pack('S')
output_file.write(num_instruments_data)

num_patterns_data = [ patterns.size ].pack('S')
output_file.write(num_patterns_data)

ticks_per_pattern_data = [ ticks_per_pattern ].pack('S')
output_file.write(ticks_per_pattern_data)

instruments.each { |instrument|
    data = [ instrument[:waveform_index], 0, 0, 0].pack('C4')
    output_file.write(data)

    data = [ instrument[:attack], instrument[:attack_gain], instrument[:decay], instrument[:sustain_gain], instrument[:release] ].pack('D*')
    output_file.write(data)
}

patterns.each { |pattern|
    # pattern header
    data = [ pattern.length ].pack('S')
    output_file.write(data)

    # sort contents of pattern based on :tick_index property
    sorted_commands = pattern.sort_by { |a| [ a[:tick_index], a[:channel] ] }

    sorted_commands.each { |command|
        data = [ command[:tick_index], 
                 command[:instrument_index], 
                 command[:channel], 
                 command[:action_type], 
                 command[:note], 
                 command[:octave] ].pack('C*')
        output_file.write(data)
    }
}

output_file.close()

