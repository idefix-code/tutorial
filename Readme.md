About this tutorial
--------------------

In this tutorial, you will learn how to use idefix on various architectures. Here, we will do everything through a Jupyter notebook opened on a GPU-hosting machine, but you can also do most of the tutorial on the CPU of your laptop.

This tutorial was prepared by Geoffroy Lesur (geoffroy.lesur@univ-grenoble-alpes.fr) and Sebastien Valat (sebastien.valat@univ-grenoble-alpes.fr).

Deployment
-----------

You should first install idefix and this tutorial on the target machine (which can be your machine). We first follow the [idefix documentation](https://idefix.readthedocs.io/latest/) to install it, and then we clone this repository and install the tutorial.

Idefix installation
++++++++++++++++++++

Assuming you want to use https to get idefix (easiest option):

```shell
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix
cd idefix
```

This will create and deploy Idefix in the directory `idefix`.


Set the `IDEFIX_DIR` environment variable to the absolute path of the directory

```shell
export IDEFIX_DIR=<idefix main folder>
```

