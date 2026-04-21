About this tutorial
--------------------

In this tutorial, you will learn how to use idefix on various architectures. Here, we will do everything through a Jupyter notebook opened on a GPU-hosting machine, but you can also do most of the tutorial on the CPU of your laptop.

This tutorial was prepared by Geoffroy Lesur (geoffroy.lesur@univ-grenoble-alpes.fr) and Sebastien Valat (sebastien.valat@univ-grenoble-alpes.fr).

Deployment
-----------

You should first install idefix and this tutorial on the target machine (which can be your machine). We first follow the [idefix documentation](https://idefix.readthedocs.io/latest/) to install it, and then we clone this repository and install the tutorial.

Idefix installation
===================

You can play around with this tutorial on a distant machine or on your laptop (for the asnum school, we recommend deploying on either the dgx, h200x4 or procamd machines). In any cases, you should clone this tutorial and idefix source code on the target machine, so that you can directly use these source files and test what you are doing. 

We first connect to the target machine (here dgx, but change it as needed to balance the charge between users) and make a tunnel to a unix socket (which will be used by our jupyter notebook)

```shell
ssh dgx.cbp.ens-lyon.fr -L 8888:/home/<your_login>/mysock.sock
```

In the directory of your choice, you then clone idefix sources and our tutorial:

```shell
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix.src
git clone --branch asnum2026 https://github.com/idefix-code/tutorial.git
```

Keep in mind the directory where `idefix.src` and `tutorial` have been deployed as these will become handy soon.

Jupyter lab deployment for the tutorial
============================================

We first create a python environement, install what we need, and launch a jupyter lab session
```shell
python3 -m venv ./idefix-env
source idefix-env/bin/activate
pip install -r tutorial/python_requirements.txt
jupyter lab --sock=~/mysock.sock
```

you're now good to go. On your laptop, using your favourite browser, open the link

```
http://localhost:8888/lab?token=<your_token>
```

Which should open your jupyter lab session running on the distant machine. From there, go to the tutorial directory noted above, and open `start.ipynb``








