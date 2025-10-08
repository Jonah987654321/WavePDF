#include "../src/utility/PdfReader.h"
#include <wx/wx.h>
#include <gtest/gtest.h>

TEST(PdfReaderIntegrationTest, SamplePDFProcess) {
    // wxWidgets needs an app instance
    wxInitializer initializer;
    ASSERT_TRUE(initializer.IsOk());

    PdfReader reader("../sample.pdf");

    // Test if the full process runs through
    EXPECT_TRUE(reader.process());

    // Test if the correct xref offset has been parsed
    EXPECT_EQ(reader.getXRefOffset(), size_t(18132));
}
