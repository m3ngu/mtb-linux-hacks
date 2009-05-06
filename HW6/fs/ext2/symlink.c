/*
 *  linux/fs/ext2/symlink.c
 *
 * Only fast symlinks left here - the rest is done by generic code. AV, 1999
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/symlink.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  ext2 symlink handling code
 */

#include "ext2.h"
#include "xattr.h"
#include <linux/namei.h>

static int ext2_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct ext2_inode_info *ei = EXT2_I(dentry->d_inode);
	nd_set_link(nd, (char *)ei->i_data);
	return 0;
}

struct inode_operations ext2_symlink_inode_operations = {
	.readlink	= generic_readlink,
	.follow_link	= page_follow_link_light,
	.put_link	= page_put_link,
#ifdef CONFIG_EXT2_FS_XATTR
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= ext2_listxattr,
	.removexattr	= generic_removexattr,
#endif
	/* HW6 additions: */
	.add_tag 	= ext2_addtag,
	.rm_tag 	= ext2_rmtag,
	.get_tags	= ext2_gettags,	
};
 
struct inode_operations ext2_fast_symlink_inode_operations = {
	.readlink	= generic_readlink,
	.follow_link	= ext2_follow_link,
#ifdef CONFIG_EXT2_FS_XATTR
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= ext2_listxattr,
	.removexattr	= generic_removexattr,
#endif
	/* HW6 additions: */
	.add_tag 	= ext2_addtag,
	.rm_tag 	= ext2_rmtag,
	.get_tags	= ext2_gettags,	
};
