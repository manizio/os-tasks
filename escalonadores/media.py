import subprocess
import re
import sys
import time

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
        print('''uso: media.py [escalonador]
            escalonadores: fifo | sjf | ljf | prio_static | prio_dynamic''')
    else:

        sched = sched_dict[sys.argv[1]]

        report = open(f'media/{sys.argv[1]}.txt', 'w')

        while NPROC <= 100:
            tme = 0
            for i in range(0, 10):
                try:
                    result = subprocess.run([sched, f'{NPROC}'], capture_output=True, timeout=5)
                    output = result.stdout.decode("utf-8")
                except subprocess.TimeoutExpired:
                    i -= 1
                    continue
                finally:
                    match = re.search(r"TME: (\d+.\d+)", output)
                    if match:
                        tme += float(match.group(1))
            print(f'{NPROC} {tme/10}')
            report.write(f'{NPROC} {tme/10}\n')

            NPROC += 10
            
            
        report.close()


    