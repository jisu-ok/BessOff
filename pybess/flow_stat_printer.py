import time

progress_bar_cnt = -1
symbols = ['|', '/', '-', '\\']
def symbol_selector():
    global progress_bar_cnt
    global symbols
    progress_bar_cnt = (progress_bar_cnt + 1) % 4
    return symbols[progress_bar_cnt]


def print_flow_stats(monitor):
    def flag_parser(isTcp, flags):
        if not isTcp:
            return "-"
        
        lst = []
        if flags & 0x20:
            lst.append("FIN")
        if flags & 0x10:
            lst.append("ACK")
        if flags & 0x08:
            lst.append("PSH")
        if flags & 0x04:
            lst.append("RST")
        if flags & 0x02:
            lst.append("SYN")
        if flags & 0x01:
            lst.append("FIN")
        return ','.join(lst)


    while True:
        time.sleep(1)

        ret = monitor.get_stats(tcp=True, udp=True)
        print("\n"*3)
        print("- - - - - - - - - - - - - - - - - - - * {0} Flow Monitor by Byeongkeon & Jisu {0} * - - - - - - - - - - - - - - - - - - \n".format(symbol_selector()))
        print("  --------------------Flows (by 5-tuple)--------------------     --------------------Statistics--------------------\n")
        print(
            "{:>15}  {:>15}  {:>7}  {:>7}  {:>8}  |  {:>15}  {:>15}  {:>10}  {}".format(
                "SrcIP", "DstIP", "SrcPort", "DstPort", "Protocol", "Packets (Mpkts)", "Bytes (GB)", "Age (ms)", "Flags")
        )
        cnt = 1
        for stats in ret.statistics:
            print(
                "{:>15}  {:>15}  {:>7}  {:>7}  {:>8}  |  {:>15.3f}  {:>15.3f}  {:>10.3f}  {}".format(
                    stats.src_ip,
                    stats.dst_ip,
                    stats.src_port,
                    stats.dst_port,
                    stats.protocol,
                    stats.packets / 1e6,
                    stats.bytes / 1e9,
                    stats.age / 1e6,
                    flag_parser(True if stats.protocol == "TCP" else False, stats.flags)
                )
            )
            cnt += 1

        emptylines = 30 - cnt
        print("\n"*emptylines)
