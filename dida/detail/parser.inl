namespace dida::detail
{

Parser::Parser(std::string_view string) : head_(string.data()), end_(string.data() + string.size())
{
}

Parser::Parser(const char* begin, const char* end) : head_(begin), end_(end)
{
}

const char* Parser::head() const
{
  return head_;
}

} // namespace dida::detail
