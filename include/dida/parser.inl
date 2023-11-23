#include <cctype>

namespace dida
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
  return try_match(c);
}

bool Parser::match(std::string_view string)
{
  for (const char c : string)
  {
    if (!match(c))
    {
      return false;
    }
  }

  return true;
}

bool Parser::try_match(char c)
{
  if (head_ == end_ || *head_ != c)
  {
    return false;
  }

  head_++;
  return true;
}

bool Parser::try_match(std::string_view string)
{
  const char* head_backup = head_;
  if (!match(string))
  {
    head_ = head_backup;
    return false;
  }

  return true;
}

namespace
{

bool is_space(char c)
{
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

} // namespace

bool Parser::skip_required_whitespace()
{
  if (head_ == end_ || !isspace(*head_))
  {
    return false;
  }

  head_++;
  skip_optional_whitespace();
  return true;
}

void Parser::skip_optional_whitespace()
{
  while (head_ != end_ && isspace(*head_))
  {
    head_++;
  }
}

std::optional<Point2> Parser::parse_point2()
{
  std::optional<Vector2> as_vector = parse_vector2();
  if (!as_vector)
  {
    return std::nullopt;
  }

  return Point2(*as_vector);
}

} // namespace dida
