# Maintainer: Max Meinhold <mxmeinhold@gmail.com>
pkgname=img
pkgver=`cat VERSION`
pkgrel=1
pkgdesc='basic libpng wrapper'
arch=('x86_64')
url='https://github.com/mxmeinhold/img'
license=('MIT')
depends=('glibc')
makedepends=('git' 'make')
source=("img-$pkgver.tar.gz" "img-$pkgver.tar.gz.sig")
sha256sums=('781269f06ac3036ee5f592c3c12c135779114f7173e512296e342c1bee13a7f5'
            'SKIP')
validpgpkeys=('B77D730E8D444707FA93320D72E05836F8252405')

build() {
    make lib
}

package() {
    mkdir -p $pkgdir/usr/include/
    mkdir -p $pkgdir/usr/lib/
    install -Dm644 LICENSE.txt "$pkgdir/usr/share/licenses/$pkgname/MIT-LICENSE"
    make DESTDIR="$pkgdir" install
}
