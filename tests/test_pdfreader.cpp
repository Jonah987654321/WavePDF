#include "../src/utility/PdfReader.h"
#include <wx/wx.h>
#include <gtest/gtest.h>

TEST(PdfReaderIntegrationTest, SamplePDFProcess) {
    // wxWidgets needs an app instance
    wxInitializer initializer;
    ASSERT_TRUE(initializer.IsOk());

    PdfReader reader("../tests/samples/sample.pdf");

    // Test if the full process runs through
    EXPECT_TRUE(reader.process()) << "PdfReader::process() failed with log: " << reader.getLog();

    // Test if the correct xref offset has been parsed
    ASSERT_EQ(reader.getXRefOffset(), size_t(18132));

    // Data for xref-table sample.pdf
    std::vector<xrefSubsection> xrefTable;
    xrefSubsection subsection0;
    subsection0.startObject = 0;
    subsection0.amountObjects = 26;
    subsection0.objects.push_back({0, 65535, 0, 'f'});
    subsection0.objects.push_back({17930, 0, 1, 'n'});
    subsection0.objects.push_back({3982, 0, 2, 'n'});
    subsection0.objects.push_back({5073, 0, 3, 'n'});
    subsection0.objects.push_back({22, 0, 4, 'n'});
    subsection0.objects.push_back({3962, 0, 5, 'n'});
    subsection0.objects.push_back({4086, 0, 6, 'n'});
    subsection0.objects.push_back({5037, 0, 7, 'n'});
    subsection0.objects.push_back({7957, 0, 8, 'n'});
    subsection0.objects.push_back({17740, 0, 9, 'n'});
    subsection0.objects.push_back({13692, 0, 10, 'n'});
    subsection0.objects.push_back({4209, 0, 11, 'n'});
    subsection0.objects.push_back({5017, 0, 12, 'n'});
    subsection0.objects.push_back({5156, 0, 13, 'n'});
    subsection0.objects.push_back({5206, 0, 14, 'n'});
    subsection0.objects.push_back({7472, 0, 15, 'n'});
    subsection0.objects.push_back({7493, 0, 16, 'n'});
    subsection0.objects.push_back({7755, 0, 17, 'n'});
    subsection0.objects.push_back({8146, 0, 18, 'n'});
    subsection0.objects.push_back({13146, 0, 19, 'n'});
    subsection0.objects.push_back({13167, 0, 20, 'n'});
    subsection0.objects.push_back({13424, 0, 21, 'n'});
    subsection0.objects.push_back({13877, 0, 22, 'n'});
    subsection0.objects.push_back({17016, 0, 23, 'n'});
    subsection0.objects.push_back({17037, 0, 24, 'n'});
    subsection0.objects.push_back({17302, 0, 25, 'n'});
    xrefTable.push_back(subsection0);
    ASSERT_EQ(reader.getXRefTable(), xrefTable);
}