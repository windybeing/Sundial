#! /bin/bash
# servers="172.31.27.114"
# servers="172.31.18.67;"
# servers="172.31.18.67;172.31.18.51;"
servers="172.31.18.67;172.31.18.51;172.31.19.108;172.31.19.248;172.31.29.15;172.31.29.225;172.31.18.112;172.31.21.130;172.31.26.193;172.31.19.196;172.31.30.165;172.31.24.231;172.31.22.232;172.31.16.149"
server_array=$(echo $servers | tr ";" "\n")
echo $server_array

run() {
for server in $server_array; do
    ssh $server "tmux kill-session -t sundial"
done
pkill rundb

for server in $server_array; do
    rsync -aqzP --port=22 --force ./ifconfig.txt $server:/home/ubuntu/Sundial/
    rsync -aqzP --port=22 --force ./rundb $server:/home/ubuntu/Sundial/
    rsync -aqzP --port=22 --force ./benchmarks/YCSB_schema.txt $server:/home/ubuntu/Sundial/benchmarks/
    rsync -aqzP --port=22 --force ./benchmarks/YCSB10_schema.txt $server:/home/ubuntu/Sundial/benchmarks/
done

for addr in $server_array; do
    ssh $addr "cd Sundial; export LD_LIBRARY_PATH=\"/home/ubuntu/Sundial/libs;$LD_LIBRARY_PATH\"; tmux new-session -d -s sundial \"./rundb -GW10 -GT10 -S6 -Yr$ratio -z$contention -R10 -r0.5 -s16454577 > cache.txt\"" &
done

./rundb -GW10 -GT10 -S6 -z$contention -Yr$ratio -R10 -r0.5 -s16454577 -o cache.txt 
}

print() {
grep Throughput cache.txt
for server in $server_array; do
    ssh $server "grep Throughput ./Sundial/cache.txt"
done
}

cat /dev/null > ycsb_result.txt
for ratio in 0;
do
    for contention in 0.1 ;
    do 
        run $contention $ratio
        # print
        echo -n "$contention $ratio " >> ycsb_result.txt
        print | sed 's/Throughput://g' | sed 's/ //g' | awk '{s += $1} END {print s}' >> ycsb_result.txt
    done
done
