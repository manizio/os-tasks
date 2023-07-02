import sys
import re
import subprocess
from statistics import pvariance


if __name__ == '__main__':
    
    NPROC = 10

    sched_dict = {
        'fifo': './main_fifo',
        'sjf': './main_sjf',
        'ljf': './main_ljf',
        'prio_static': './main_prio_static',
        'prio_dynamic': './main_prio_dynamic'
    }

    if (len(sys.argv) != 2 or sys.argv[1] not in sched_dict.keys()):
        print('''uso: variancia.py [escalonador]
            escalonadores: fifo | sjf | ljf | prio_static | prio_dynamic''')
    else:

        sched = sched_dict[sys.argv[1]]

        report = open(f'variancia/{sys.argv[1]}.txt', 'w')

        variancia = 0

        while NPROC <= 100:
            tme_array = []

            for i in range(10):
                try:
                    result = subprocess.run([sched, f'{NPROC}'], capture_output=True, timeout=5)
                    output = result.stdout.decode("utf-8")

                except subprocess.TimeoutExpired:
                    i -= 1
                    continue
                finally:
                    match = re.search(r"TME: (\d+.\d+)", output)
                    if match:
                        tme_array.append(float(match.group(1)))

            variancia = pvariance(tme_array)
            print(f'{NPROC} {variancia}')
            report.write(f'{NPROC} {variancia}\n')
            
            NPROC += 10
        report.close()