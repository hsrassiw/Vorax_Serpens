#  Vorax Serpens

##  Giới Thiệu

- **Họ và tên**: Phạm Minh Nghĩa
- **Lớp**: K69I_CS4 - UET - VNU
- **Mã sinh viên**: 24021584
- **Bài tập lớn**: Vorax Serpens *(INT2215 7 - Lập trình nâng cao)*

##  Mô Tả

**Vorax Serpens** là một trò chơi **rắn săn mồi** cổ điển được phát triển bằng ngôn ngữ C++ và sử dụng thư viện SDL2.

 **Cơ chế chính:**
- Người chơi điều khiển một con rắn di chuyển trên màn hình để ăn thức ăn và tăng chiều dài.
- Rắn sẽ **tự động di chuyển** theo hướng hiện tại.
- Sử dụng các phím **W**, **A**, **S**, **D** hoặc **phím mũi tên** để thay đổi hướng di chuyển của rắn.
- Mỗi lần ăn thức ăn, rắn sẽ **dài ra** và điểm số của người chơi sẽ tăng lên.
- Trò chơi kết thúc nếu rắn đâm vào chính nó hoặc vào tường (nếu có).

---

##  Hướng Dẫn Cài Đặt & Chạy Game

### 1️. Cài Đặt CLion

1. Tải và cài đặt **CLion** từ [JetBrains](https://www.jetbrains.com/clion/).
2. Mở CLion và đăng nhập vào tài khoản JetBrains để kích hoạt phần mềm.

### 2️. Tải Mã Nguồn

1. Clone repository từ GitHub:
   ```sh
    git clone https://github.com/hsrassiw/Vorax_Serpens.git
   ```
2. Nếu tải file `.zip`, hãy **giải nén** thư mục dự án.
3. Mở **CLion** và chọn `Open` để mở thư mục chứa dự án.

### 3️. Cài Đặt Thư Viện Cần Thiết

Dự án sử dụng các thư viện SDL2 sau:

* `SDL2`: thư viện chính cho đồ họa và xử lý sự kiện
* `SDL2_image`: hỗ trợ xử lý ảnh (PNG, JPG, v.v.)
* `SDL2_mixer`: xử lý âm thanh
* `SDL2_ttf`: hiển thị văn bản với font TrueType

####  Windows:

1. Tải xuống từ trang chính thức:
    * [SDL2](https://github.com/libsdl-org/SDL/releases)
    * [SDL2_image](https://github.com/libsdl-org/SDL_image/releases)
    * [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases)
    * [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases)
2. Giải nén và cấu hình thư viện trong CLion:
    * Copy các thư mục `include` vào nơi phù hợp.
    * Cập nhật `CMakeLists.txt` để chỉ đường tới file `.lib`.
    * Đảm bảo các file `.dll` nằm cùng thư mục với file `.exe` khi chạy game.

 *Lưu ý*: Người dùng Windows cần tự cấu hình thư viện trong `CMakeLists.txt` nếu không dùng gói quản lý như vcpkg hoặc MSYS2.

### 4️. Build & Chạy Game

1. Đảm bảo file `CMakeLists.txt` được nhận diện trong CLion.
2. Nhấn **Build** để biên dịch game.
3. Chạy game bằng cách nhấn **Run** hoặc thực thi file `.exe` (Windows) .

---

##  Nguồn tài nguyên (Credits)

Dự án này sử dụng các tài nguyên từ bên thứ ba. Dưới đây là thông tin về các nguồn tài nguyên:

*   **Âm thanh**
    *   Âm thanh khi ăn: **BLING by MSH443PL8** -- <https://freesound.org/s/796125/> -- Giấy phép: [Creative Commons 0 (CC0)](https://creativecommons.org/publicdomain/zero/1.0/).
    *   Âm thanh Game Over (UI Decline/Back) từ [OpenGameArt.org](https://opengameart.org/content/ui-decline-or-back), được tạo bởi [David Mckee (ViRiX)](https://soundcloud.com/virix) – sử dụng theo giấy phép [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).
    *   Âm thanh va chạm từ [TiengDong.com](https://tiengdong.com/am-thanh-va-cham-hoat-hinh).

*   **Hình ảnh / Sprite**
    *   Một số hình ảnh pixel(mồi, main menu) được tạo bằng công cụ AI [PixelLab.ai](https://www.pixellab.ai/editor).
    *   Ảnh nền không gian được tạo bằng [Space Background Generator](https://deep-fold.itch.io/space-background-generator) của Deep-Fold – tài nguyên miễn phí từ [Itch.io](https://itch.io/).

*   **Phông chữ**
    *   [Roboto](https://fonts.google.com/specimen/Roboto) – phông chữ mã nguồn mở từ Google Fonts.

---

##  Nguồn Tham Khảo

Trong quá trình phát triển dự án này, em đã tham khảo các nguồn tài liệu và hướng dẫn sau:

*   **Tài liệu SDL2 :**
    *   [SDL2 Wiki](https://wiki.libsdl.org/SDL2/FrontPage): Trang web chính thức của nhà phát triển.
    *   [LTNC](https://docs.google.com/document/d/1FZ3jTqHxtyZznNWiJmmve0zYu_aSliUqLP2OsMcdehQ/edit?tab=t.0#heading=h.g69r5ua9lzeh): Tài liệu bổ sung cho học phần.
*   **Hướng dẫn & Tutorials:**
    *   [Lazy Foo' Productions - SDL2 Tutorials](https://lazyfoo.net/tutorials/SDL/): Một chuỗi bài hướng dẫn về cách sử dụng SDL2 để lập trình game.
    *   [Parallel Realities - SDL Tutorials](https://www.parallelrealities.co.uk/tutorials/): Các bài hướng dẫn về lập trình game với SDL. 
    *   [Cppreference](https://en.cppreference.com/w/): Trang tra cứu chuẩn về ngôn ngữ C++ và thư viện chuẩn.
    *   [Markdown Guide](https://www.markdownguide.org/basic-syntax/): Hướng dẫn về cú pháp Markdown.
    

