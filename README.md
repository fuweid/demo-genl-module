## demo-genl-module

This is repo used to create generic netlink handler like taskstats in kernel.
The generic netlink handler is named by `CTRL_DEMOGENL`, which can be used for
any kind of generic netlink client.

> NOTE: The generic netlink handler works well in
`Ubuntu 16.04.4 LTS (Xenial Xerus)` which run in
`Linux ubuntu-xenial 4.4.0-166-generic`.

### How to

With the following steps, you will register kernel module.

```shell
$ git clone https://github.com/fuweid/demo-genl-module.git
$ cd demo-genl-module
$ make # checkout make help for more actions
$ sudo insmod demo-module.ko
```

Run dmesg and will see

```shell
$ dmesg | tail -n 1
... registered generic netlink CTRL_DEMOGENL module.
```

And use `genl ctrl get name CTRL_DEMOGENL` to see more detail.

```shell
$ genl ctrl get name CTRL_DEMOGENL                                                                                                                                                                                           255 master!

Name: CTRL_DEMOGENL
        ID: 0x12  Version: 0x1  header size: 0  max attribs: 1
	commands supported:
		#1:  ID-0x1
```

### TODO

* add help for handler
* add userspace application for demo
* consume message from userspace
