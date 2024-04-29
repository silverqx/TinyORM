# Gentoo `crystal` repository

Copy `crystal/` folder to the `/var/db/repos/`

```bash
sudo cp -r crystal /var/db/repos
cd /var/db/repos/crystal
sudo chown -R root:root .
```

Enable `crystal` repository by creating the `crystal-repo.conf` at `/etc/portage/repos.conf/`

```ini
[crystal]
location = /var/db/repos/crystal
```

Repository also contains the [`tabulate`](https://github.com/p-ranav/tabulate) package because `gentoo` repository doesn't have one.

Enable unstable `~amd64` keyword for `tinyorm` and `tabulate` packages by creating `tinyorm` file at `/etc/portage/package.accept_keywords/`

```
dev-db/tinyorm ~amd64
dev-cpp/tabulate ~amd64
```

Configure `USE` flags by creating `tinyorm` file at `/etc/portage/package.use/`

```
dev-db/tinyorm mysql postgres sqlite mysql-ping tom tom-cli -qt5
#dev-db/tinyorm build-drivers mysql -sqlite -qt5
```

Check `USE` flags

```bash
equery uses tinyorm
```

Install `tinyorm` package

```bash
sudo emerge --quiet-build -va tinyorm
```
