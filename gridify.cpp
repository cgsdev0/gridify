#include <fcntl.h>
#include <locale.h>

#include <iomanip>
#include <iostream>
#include <locale>
#include <vector>

namespace box {
const wchar_t N = L' ';        // None
const wchar_t V = L'\u2503';   // Vertical
const wchar_t H = L'\u2501';   // Horizontal
const wchar_t VH = L'\u254B';  // Cross
const wchar_t UR = L'\u2517';  // Bottom left corner
const wchar_t DR = L'\u250F';  // Top left corner
const wchar_t UL = L'\u251B';  // Bottom right corner
const wchar_t DL = L'\u2513';  // Top right corner
const wchar_t HD = L'\u2533';  // Horizontal / Down
const wchar_t HU = L'\u253B';  // Horizontal / Up
const wchar_t VR = L'\u2523';  // Vertical / Right
const wchar_t VL = L'\u252B';  // Vertical / Left
}  // namespace box

struct Cell {
    Cell() : longest_line(0) {}
    std::vector<std::string> lines;
    unsigned int longest_line;
};

struct Row {
    Row() : tallest_cell(0) {}
    std::vector<Cell> cells;
    unsigned int tallest_cell;
};

std::string centerText(const std::string& str, int colWidth) {
    std::string result(str);
    while (result.length() < colWidth) {
        if (result.length() % 2) {
            result.push_back(' ');
        } else {
            result = " " + result;
        }
    }
    return result;
}

void printHorizontalDivider(const std::vector<int>& columnWidths, bool start,
                            bool end) {
    auto fill = std::wcout.fill();
    if (start) {
        std::wcout << box::DR;
    } else if (end) {
        std::wcout << box::UR;
    } else {
        std::wcout << box::VR;
    }
    std::wcout << std::setfill(box::H);
    for (int i = 0; i < columnWidths.size(); ++i) {
        if (i == columnWidths.size() - 1) {
            std::wcout << std::setw(columnWidths[i]) << "";
            if (start) {
                std::wcout << box::DL;
            } else if (end) {
                std::wcout << box::UL;
            } else {
                std::wcout << box::VL;
            }
        } else {
            std::wcout << std::setw(columnWidths[i]) << "";
        }
        if (i < columnWidths.size() - 1) {
            if (start)
                std::wcout << std::setw(0) << box::HD;
            else if (end)
                std::wcout << std::setw(0) << box::HU;
            else
                std::wcout << std::setw(0) << box::VH;
        }
    }
    std::wcout << std::setfill(fill) << std::endl;
}
int main(int argc, char** argv) {
    bool useHeaders = true;         // TODO: make this an optional flag
    bool usePadding = true;         // TODO: make this an optional flag
    bool normalizeColumns = false;  // TODO: make this an optional flag

    constexpr char locale_name[] = "en_US.utf8";
    setlocale(LC_ALL, locale_name);
    std::locale::global(std::locale(locale_name));
    std::wcout.imbue(std::locale());

    if (argc != 2) {
        std::wcout << "Usage: " << argv[0] << " [cols]" << std::endl;
        return 1;
    }
    std::string columnCountStr(argv[1]);
    int columnCount = stoi(columnCountStr);

    std::string line;
    std::vector<Row> grid;
    Row row;
    Cell cell;
    int i = 0;
    while (std::getline(std::cin, line)) {
        if (line.length() == 0) {
            row.cells.push_back(cell);
            if (cell.lines.size() > row.tallest_cell) {
                row.tallest_cell = cell.lines.size();
            }
            // New cell
            cell = Cell();
            if (++i >= columnCount) {
                grid.push_back(row);
                i = 0;
                row = Row();
            }
        } else {
            if (usePadding) {
                // add horizontal padding
                line = " " + line + " ";
            }
            cell.lines.push_back(line);
            if (line.length() > cell.longest_line) {
                cell.longest_line = line.length();
            }
        }
    }
    if (cell.lines.size()) {
        row.cells.push_back(cell);
    }
    if (row.cells.size()) {
        while (row.cells.size() < columnCount) {
            row.cells.push_back(Cell());
        }
        grid.push_back(row);
    }

    std::vector<int> columnWidths;
    int biggestCol = 0;
    // compute column widths
    for (i = 0; i < grid[0].cells.size(); ++i) {
        int widestCol = 0;
        for (int j = 0; j < grid.size(); ++j) {
            if (grid[j].cells.size() <= i) continue;
            if (grid[j].cells[i].longest_line > widestCol) {
                widestCol = grid[j].cells[i].longest_line;
            }
        }
        columnWidths.push_back(widestCol);
        if (widestCol > biggestCol) {
            biggestCol = widestCol;
        }
    }

    if (normalizeColumns) {
        // Normalize all of the column widths
        for (i = 0; i < columnWidths.size(); ++i) {
            columnWidths[i] = biggestCol;
        }
    }

    // Print row by row
    for (int row_index = 0; row_index < grid.size(); ++row_index) {
        auto row = grid[row_index];
        printHorizontalDivider(columnWidths, row_index == 0, false);
        for (i = 0; i < row.tallest_cell; ++i) {
            std::wcout << box::V;
            for (int j = 0; j < row.cells.size(); ++j) {
                if (row.cells[j].lines.size() <= i) {
                    std::wcout << std::left << std::setw(columnWidths[j]) << "";
                } else {
                    if (useHeaders && i == 0) {
                        std::wcout << std::setw(0)
                                   << centerText(row.cells[j].lines[i],
                                                 columnWidths[j])
                                          .c_str();
                    } else {
                        std::wcout << std::left << std::setw(columnWidths[j])
                                   << row.cells[j].lines[i].c_str();
                    }
                }
                if (j < columnCount - 1) {
                    std::wcout << box::V;
                }
            }
            std::wcout << box::V << std::endl;
            if (useHeaders && i == 0) {
                printHorizontalDivider(columnWidths, false, false);
            }
        }
    }
    printHorizontalDivider(columnWidths, false, true);

    return 0;
}
