import time

alpha = 0.125
offload_min_time = 5

def agent(measurer, off_points, rep_type='median', spike_percent=20, fall_percent=30):
    est_latency_usual = None # estimated latency, which is EWMA (exponential weighted moving average)
    est_latency_unusual = None # similarly, EWMA

    clock = 0
    offloaded = False
    offload_timer = 0

    while True:
        time.sleep(1)
        clock += 1
        print("[clock={}] ".format(clock), end='')

        ret = measurer.get_summary(clear=True, latency_percentile=[50,99])
        if rep_type == 'avg':
            val = ret.latency.avg_ns
        elif rep_type == 'median':
            val = ret.latency.percentile_values_ns[0]
        elif rep_type == '99th':
            val = ret.latency.percentile_values_ns[1]
        print("Measured latency value ({}): {} (ns)".format(rep_type, val))

        if offloaded: # offlaoded mode
            offload_timer += 1
            if est_latency_unusual - val > est_latency_unusual * fall_percent / 100 and offload_timer >= offload_min_time:
                # onload again
                for p in off_points:
                    p.set_ogate(0)
                    print("Onloaded a PointMachine!")
                    print(p)
                offloaded = False
                est_latency_usual = None
            else:
                est_latency_unusual = (1 - alpha) * est_latency_unusual + alpha * val
        
        else: # onloaded mode    
            if est_latency_usual == None:
                est_latency_usual = val
                continue
            
            if val - est_latency_usual > est_latency_usual * spike_percent / 100:
                # latency spike -> offload
                for p in off_points:
                    p.set_ogate(1)
                    print("Offloaded a PointMachine!")
                    print(p)
                offloaded = True
                offload_timer = 0
                est_latency_unusual = val
            else:
                est_latency_usual = (1 - alpha) * est_latency_usual + alpha * val

        print()

    return

