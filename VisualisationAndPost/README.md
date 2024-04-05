# Introduction


These tutorials requires Python 3.8 or newer, plus a few additional packages. It is strongly advised to create a virtual
environment before installing anything specifically for these tutorials. To get
started, navigate to the tutorial subdirectory

```shell
$ cd idefix-tutorial/VisualisationAndPost/
```

and create a virtual environment...

... with conda
```shell
$ conda create --name=<name> python=3.10
$ conda activate <name>
```
(`<name>` is a placeholder, for instance `idefix_tuto`, and the Python version is
indicative)

... or with `venv`
```shell
$ python -m venv .venv
$ source .venv/bin/activate
```

Then proceed to install requirements
```shell
$ python -m pip install -r requirements.txt
```

### Cleaning the mess

Between sessions, and before we move on, you might want to clean up your shell and your
disk ! Here's how to.

#### Deactivate your virtual env

on conda
```shell
$ conda deactivate
```

or with `venv`
```shell
$ source deactivate
```

#### Remove the virtual env
with conda
```shell
$ conda remove --all --name <name>
```

or with `venv`
```shell
$ rm -fr .venv
```
