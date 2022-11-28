#include <cctype>

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

const char* Parser::end() const
{
  return end_;
}

bool Parser::finished() const
{
  return head_ == end_;
}

bool Parser::match(char c)
{
  if(head_ == end_ || *head_ != c)
  {
    return false;
  }

  head_++;
  return true;
}

void Parser::skip_optional_whitespace()
{
  while(head_ != end_ && std::isspace(*head_))
  {
    head_++;
  }
}

std::optional<Point2> Parser::parse_point2()
{
  std::optional<Vector2> as_vector = parse_vector2();
  if(!as_vector)
  {
    return std::nullopt;
  }

  return Point2(*as_vector);
}

} // namespace dida::detail
