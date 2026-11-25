# Offline Renderer

Cornell Box を描画する、C++17 製の CPU パストレーサーです。画素の計算を複数のスレッドに分割し、境界ボリューム階層（BVH）でレイと形状の交差判定を高速化します。直接照明は発光メッシュ上の点をサンプリングして求め、間接照明のパスはロシアンルーレットで確率的に打ち切ります。

既定の解像度は **784 × 784 ピクセル**で、1 ピクセルあたり **128 サンプル**を使用します。

## ビルドと実行

CMake 3.20 以降と、C++17 に対応したコンパイラが必要です。以下のコマンドは、`CMakeLists.txt` があるプロジェクトのルートディレクトリから実行してください。

### Windows（Visual Studio 2022）

Visual Studio Installer で「C++ によるデスクトップ開発」ワークロードをインストールし、開発者用 PowerShell で実行します。

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
Set-Location build
.\Release\offline_renderer.exe
```

### Linux / macOS

単一構成の CMake ジェネレーターを使用する場合は、次のコマンドで Release ビルドを実行できます。

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cd build
./offline_renderer
```

## 出力画像

CMake は Cornell Box を構成する 6 個の OBJ ファイルを `build/models/cornellbox/` にコピーします。レンダラーは作業ディレクトリからの相対パスでモデルを読み込むため、`build` ディレクトリに移動してから実行してください。

出力先は `build/binary.ppm` です。画像はバイナリ PPM（P6）形式で保存され、実行するたびに同名のファイルが上書きされます。

## ソース構成

| パス | 役割 |
| --- | --- |
| `src/Main.cpp` | Cornell Box のシーン構築とプログラムのエントリーポイント |
| `src/Renderer.cpp` | カメラレイの生成、画素の並列処理、PPM 画像の書き出し |
| `src/Scene.cpp` | 光源のサンプリングと再帰的なパストレーシング |
| `src/BVH.cpp` | BVH の構築と走査 |
| `src/MeshTriangle.hpp`、`src/Triangle.hpp` | OBJ メッシュの読み込みと三角形の交差判定 |
| `src/models/` | Cornell Box のモデルと、既定のシーンでは使用しないウサギのサンプルモデル |

同梱の OBJ ローダーのライセンスは MIT License です。詳細は [OBJ ローダーのライセンス](src/OBJ_Loader_LICENSE.txt) を参照してください。
