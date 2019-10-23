#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define DEMOGENL_VERSION 1
#define DEMOGENL_NAME "CTRL_DEMOGENL"

// Define attributes
//
// The index in the following enum is used as a reference for the rta_type in
// rtattr. The userspace application must to set the corresponding type by the
// index. Note the index 0 is never used. And the payload for the corresponding
// type will be defined in nla_policy.
//
// REF: include/uapi/linux/rtnetlink.h
enum {
    DEMOGENL_ATTR_UNSPEC,
    DEMOGENL_ATTR_ECHO_MSG,
    __DEMOGENL_ATTR_MAX,
};
#define DEMOGENL_ATTR_MAX (__DEMOGENL_ATTR_MAX - 1)

// Define policy for attribute
//
// The nla_policy is used to define what is the payload in attribute. The
// NLA_NUL_STRING is a variable length NULL terminated string.
//
// From https://wiki.linuxfoundation.org/networking/generic_netlink_howto
//
// NLA_UNSPEC: Undefined type
// NLA_U8: An 8-bit unsigned integer
// NLA_U16: A 16-bit unsigned integer
// NLA_U32: A 32-bit unsigned integer
// NLA_U64: A 64-bit unsigned integer
// NLA_FLAG: A simple boolean flag
// NLA_MSECS: A 64-bit time value in msecs
// NLA_STRING: A variable length string
// NLA_NUL_STRING: A variable length NULL terminated string
// NLA_NESTED: A stream of attributes
//
// More payloads are defined in include/net/netlink.h for policy.
static struct nla_policy demogenl_policy[DEMOGENL_ATTR_MAX + 1] = {
    [DEMOGENL_ATTR_ECHO_MSG] = { .type = NLA_NUL_STRING },
};

// Define generic family
//
// The name field is used for userspace application to retrieve the family id
// before sending request.
static struct genl_family demogenl_family = {
    // NOTE: depend on kernel version, old version using GENL_ID_GENERATE for
    // id
    .id = GENL_ID_PMCRAID,
    // no family specific header, hdrsize is zero
    .hdrsize = 0,
    .name = DEMOGENL_NAME,
    .version = DEMOGENL_VERSION,
    .maxattr = DEMOGENL_ATTR_MAX,
};

// Define commands
//
// Like attributes, the commands are described by the following enum. And
// the corresponding implementation will be defined in genl_ops.
enum {
    DEMOGENL_CMD_UNSPEC,
    DEMOGENL_CMD_ECHO,
    __DEMOGENL_CMD_MAX,
};
#define DEMOGENL_CMD_MAX (__DEMOGENL_CMD_MAX - 1)

int demogenl_cmd_echo(struct sk_buff *skb_2, struct genl_info *info);

// Define generic netlink operations for the specific command
//
// The policy describes how to unpack wired data for the operations.
struct genl_ops demogenl_ops_echo = {
    .cmd = DEMOGENL_CMD_ECHO,
    .flags = 0,
    .policy = demogenl_policy,
    .doit = demogenl_cmd_echo,
    .dumpit = NULL,
};

// demogenl_cmd_echo implements for DEMOGENL_CMD_ECHO.
int demogenl_cmd_echo(struct sk_buff *skb_2, struct genl_info *info) {
    int rc = 0;
    struct sk_buff *skb;
    void *msg_head;

    if (info == NULL) {
        printk("unexpected nil value of genl_info\n");
        rc = -EBADR;
        goto failure;
    }

    skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    if (skb == NULL) {
        rc = -ENOMEM;
        goto failure;
    }

    msg_head = genlmsg_put(skb, 0, info->snd_seq, &demogenl_family, 0, DEMOGENL_CMD_ECHO);
    if (msg_head == NULL) {
        rc = -ENOMEM;
        goto failure;
    }

    rc = nla_put_string(skb, DEMOGENL_ATTR_ECHO_MSG, "echo from kernel space");
    if (rc != 0) {
        goto failure;
    }
    genlmsg_end(skb, msg_head);

    rc = genlmsg_unicast(genl_info_net(info), skb, info->snd_portid);
    if (rc != 0) {
        goto failure;
    }
    return 0;

failure:
    printk("error occured in DEMOGENL_CMD_ECHO: errno=%d\n", rc);
    return rc;
}

static int __init demogenl_init(void) {
    int rc;

    // NOTE: old kernel version will use genl_register_ops for operations.
    demogenl_family.ops = &demogenl_ops_echo;
    demogenl_family.n_ops = 1;

    rc = genl_register_family(&demogenl_family);
    if (rc != 0) {
        printk("error occured while register generic netlink %s module: errno=%d\n", DEMOGENL_NAME, rc);
        return rc;
    }

    printk("registered generic netlink %s module.\n", DEMOGENL_NAME);
    return 0;
}

static void __exit demogenl_exit(void) {
    int rc = genl_unregister_family(&demogenl_family);
    if (rc != 0) {
        printk("error occured while unregister generic netlink %s module: errno=%d\n", DEMOGENL_NAME, rc);
        return;
    }
    printk("unregistered generic netlink %s module.\n", DEMOGENL_NAME);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wei Fu <fuweid89@gmail.com>");
module_init(demogenl_init);
module_exit(demogenl_exit);
