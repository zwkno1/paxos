# paxos
a simple paxos daemon

## run 

* 1. init database

    run ./initdb.sh to init 3 acceptor db(acceptor1 acceptor2 acceptor3) and 3 proposer db(proposer_10001 proposer_10002 proposer_10003)

* 2. run acceptors
    ```shell
    ./server acceptor acceptor1
    ./server acceptor acceptor2
    ./server acceptor acceptor3
    ```

* 3. run proposers

    ```shell
    ./server proposer proposer_10001 yourproposalvalue
    ./server proposer proposer_10002 yourproposalvalue
    ./server proposer proposer_10003 yourproposalvalue
    ```

## to do
need add config

