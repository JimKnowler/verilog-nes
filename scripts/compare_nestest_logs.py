import argparse
import sys

def parse_args():
    parser = argparse.ArgumentParser(description='comparse nestest logs')
    parser.add_argument('--expected', '-e', required=True, dest='expected', help='filepath of expected log')
    parser.add_argument('--actual', '-a', required=True, dest='actual', help='filepath of actual log to compare with' )
    args = parser.parse_args()

    return args

def compare_logs(expected_lines, actual_lines):
    line_index = 0

    # each field is described as
    #   { label : (index, size) }
    FIELDS = {
        "pc" : (0,4),
        "opcode": (7,8),
        "A" : (48,4),
        "X" : (53,4),
        "Y" : (58,4),
        "P" : (63,4),
        "SP" : (68,4)
    }

    expected_num_lines = len(expected_lines)
    actual_num_lines = len(actual_lines)

    while (line_index < min(expected_num_lines, actual_num_lines) ):
        expected = expected_lines[line_index]
        actual = actual_lines[line_index]

        for field in FIELDS:
            (index, size) = FIELDS[field]

            expected_value = expected[index:index+size]
            actual_value = actual[index:index+size]
            if actual_value != expected_value:
                line_number = line_index + 1
                print("difference at line ", line_number)
                print("[%s] expected [%s] actual [%s]" % (field, expected_value, actual_value))

                start_line = max(0, line_index - 10)
                for i in range(start_line, start_line+10):
                    print("%6d            %s" % (i+1, expected_lines[i][0:73]))
                
                print("%6d  expected  %s <<" % (line_number, expected[0:73]))
                print("%6d    actual  %s <<" % (line_number, actual[0:73]))
                

                sys.exit(2)

        line_index += 1
    

    if expected_num_lines != actual_num_lines:
        print("different number of lines - expected [%d] != actual [%d]" % (expected_num_lines, actual_num_lines))
        sys.exit(2)

def main():
    args = parse_args()

    with open(args.expected, 'r') as expected:
        expected_lines = expected.readlines()
    
    with open(args.actual, 'r') as actual:
        actual_lines = actual.readlines()
    
    compare_logs(expected_lines, actual_lines)

if __name__ == "__main__":
    main()