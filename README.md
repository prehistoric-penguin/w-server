# w-server

A hibrid service maked up with Facebook backend stack.

# dependencies

* folly
* proxygen
* wangle
* fbthrift
* and dependencies of them

# build

```sh
mkdir build
cd build
cmake ..
make -j4
make dist

```

# feature

## http service

### git version
```sh
curl http://localhost:12306/version

{"commit_message":"    fix thrift test client, wait for all response callback finishing","commit_hash":"commit 1d2b5d70dfdc1c2e696375f4fb3eea5023b40d2e (HEAD, master)","author":"Author: prehistoric-penguin <invalid_ms_user@live.com>","commit_date":"Date:   Sat Nov 5 20:26:44 2016 +0800"}
```

### qps, request latency histogram for nearly real time
```sh
curl http://localhost:12306/monitor?command=stats

{"op_mul":{"dist80":6,"max":10,"min":0,"qps":546,"dist99":9,"avg":0,"accu":16405},"chaos":{"dist80":0,"max":0,"min":0,"qps":0,"dist99":0,"avg":0,"accu":0},"op_sub":{"dist80":6,"max":20,"min":0,"qps":542,"dist99":9,"avg":0,"accu":16281},"op_div.fin":{"dist80":6,"max":10,"min":0,"qps":545,"dist99":9,"avg":0,"accu":16367},"op_add":{"dist80":6,"max":10,"min":0,"qps":554,"dist99":9,"avg":0,"accu":16623}}
```

## thrift service

### refer test/test\_thrift.cpp for example

# intruduction

├── cmake\_config	# config file for cmake
├── distribute		# distribute config for make up deb package
│   └── conf
├── protocol		# thrift protocol for core business
├── scripts		# util script for compile thrift file, version.cpp generating
├── src			# source code tree
│   ├── gen-cpp2	# generated source, by thrift comipler
│   ├── http\_service	# http service source, for stats and version quering
│   ├── monitor		# a timeseries stats histogram
│   ├── thrift\_service	# thrift service handler
│   └── version		# generated version message file
└── test		# unit test
