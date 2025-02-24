

TEST(SystemTest, OutputWithUrl) {
  Printer printer;

  // Redirect stdout
  std::ostringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

  // Run linter and capture output
  printer.Print("Linter found issues!");

  // Restore stdout
  std::cout.rdbuf(old);

  // Verify that some output was produced
  EXPECT_FALSE(buffer.str().empty()) << "Printer produced no output!";
}